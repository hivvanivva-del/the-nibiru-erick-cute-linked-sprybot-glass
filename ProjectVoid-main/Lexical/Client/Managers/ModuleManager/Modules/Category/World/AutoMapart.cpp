#include "AutoMapart.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../../../../Libs/stb_image.h"
#include "../../../../../../Utils/FileUtil.h"

AutoMapart::AutoMapart() : Module("AutoMapart", "Makes making maparts easier!, .automapart", Category::MISC) {
	registerSetting(new BoolSetting("AutoBuild", "Builds for you", &autoBuild, autoBuild));
	registerSetting(new BoolSetting("No Render", "No Render the map", &noRender, noRender));
	registerSetting(new SliderSetting<int>("Build Range", "NULL", &buildRange, buildRange, 1, 8));
	registerSetting(new SliderSetting<int>("Place Delay", "NULL", &placeDelay, placeDelay, 0, 20));
	registerSetting(new SliderSetting<int>("BPT", "Blocks per tick", &blocksPerTick, blocksPerTick, 1, 10));
	registerSetting(new SliderSetting<int>("Ratio", "Change the similarity of color\nHigher will choose block has better/same color", &ratio, 100, 1, 100));
	registerSetting(new BoolSetting("Swap", "Switches to the block", &swap, swap));
	registerSetting(new BoolSetting("MobEquipment", "Sends MobEquipment packets to switch", &mobEquipment, false, [&]() -> bool {
		return swap;
		}));
	registerSetting(new BoolSetting("Silent", "Silent swapping", &silent, silent, [&]() -> bool {
		return swap;
	}));
}

void AutoMapart::onEnable() {
	if (startPos == BlockPos(0, -32000, 0)) Game::DisplayClientMessage("%sAutoMapart:%s Right click to start.", MCTF::WHITE, MCTF::GRAY);
}

bool AutoMapart::loadImage(std::vector<unsigned char>& image, const std::string& filename, int& x, int& y) {
	int n;
	unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 4);
	if (data != nullptr) image = std::vector<unsigned char>(data, data + x * y * 4);
	stbi_image_free(data);
	return (data != nullptr);
}

float AutoMapart::getDifference(const UIColor& c1, const UIColor& c2) {
	int dR = c1.r - c2.r;
	int dG = c1.g - c2.g;
	int dB = c1.b - c2.b;
	return std::sqrtf(dR * dR + dG * dG + dB * dB);
}

UIColor AutoMapart::getItemColor(ItemStack* item) {
	if (item == nullptr) return UIColor(0, 0, 0, 0);
	if (!InventoryUtil::isBlock(item)) return UIColor(0, 0, 0, 0);
	Block* block = ((Block*)item->mBlock);
	if (!block) return UIColor(0, 0, 0, 0);
	mce::Color itemColor = *(mce::Color*)((uintptr_t)block->blockLegacy + 0x18C);
	return ColorUtil::toUIColor(itemColor);
}

uint8_t AutoMapart::getItem(const UIColor& color) {
	uint8_t result = -1;
	UIColor colorValue = UIColor(0, 0, 0);
	if (InventoryUtil::isBlock(InventoryUtil::getItem(InventoryUtil::getSelectedSlot()))) colorValue = getItemColor(InventoryUtil::getItem(InventoryUtil::getSelectedSlot()));
	float closestColor = 999.f;
	for (int i = 0; i < 9; i++) {
		ItemStack* item = InventoryUtil::getItem(i);
		if (!InventoryUtil::isValid(item)) continue;
		if (InventoryUtil::isBlock(item)) {
			UIColor blockColor = getItemColor(item);
			float colorDifference = getDifference(color, blockColor);
			if ((colorDifference < closestColor) && colorDifference <= 100.f / ratio) {
				closestColor = colorDifference;
				result = i;
			}
		}
	}
	return result;
}

void AutoMapart::onNormalTick(LocalPlayer* localPlayer) {
	static float delay = 0.f;
	UIColor handColor = getItemColor(InventoryUtil::getItem(InventoryUtil::getSelectedSlot()));
	if (image.empty()) {
	    std::string filename = FileUtil::getClientPath() + "Maparts\\" + mapartName + ".png";
		int width, height;
		bool success = loadImage(image, filename, width, height);
		if (!success) {
			Game::DisplayClientMessage("AutoMapart: %sFailed to load image %s%s%s.", MCTF::GRAY, MCTF::WHITE, (mapartName + ".png").c_str(), MCTF::GRAY);
			return;
		}
		const size_t RGBA = 4;
		for (int x = 0; x <= width - 1; x++) {
			for (int z = 0; z <= height - 1; z++) {
				size_t index = RGBA * (z * width + x);
				UIColor color = UIColor(image[index + 0], image[index + 1], image[index + 2], 135);
				BlockPos renderPos = startPos.add(BlockPos(x, 0, z));
				mapArt[renderPos] = color;
			}
		}
	}
	static std::vector<BlockPos> buildList;
	buildList.clear();
	const Vec3<float>& actorPos = localPlayer->getPos().floor().sub(0, 1, 0);
	auto inList = [&](const BlockPos& blockPos) -> bool {
		if (buildList.empty()) return false;
		std::vector<BlockPos>::iterator it = std::find_if(buildList.begin(), buildList.end(), [blockPos](const BlockPos& block) {
			return block == blockPos;
		});
		return it != buildList.end();
	};
	if (!autoBuild) return;
	for (int x = -buildRange; x <= buildRange; x++) {
		for (int y = -buildRange; y <= buildRange; y++) {
			for (int z = -buildRange; z <= buildRange; z++) {
				const BlockPos& blockPos = actorPos.CastTo<int>().add(x, y, z);
				if (!inList(blockPos)) buildList.push_back(blockPos);
			}
		}
	}
	if (buildList.empty()) return;
	std::sort(buildList.begin(), buildList.end(), [actorPos](BlockPos b1, BlockPos b2) {
		return WorldUtil::distanceToBlock(actorPos, b1) < WorldUtil::distanceToBlock(actorPos, b2);
	});
	int c = 0;
	uint8_t oldSlot = InventoryUtil::getSelectedSlot();
	if (TimerUtil::hasReached<ticks>(&delay, placeDelay)) {
		for (const BlockPos& block : buildList) {
			if (!mapArt[block].has_value()) continue;
			if (!PlayerUtil::canPlaceBlock(block)) continue;
			UIColor color = mapArt[block].value();
			uint8_t bestSlot = getItem(color);
			if (InventoryUtil::getSelectedSlot() != bestSlot && bestSlot != -1 && swap) {
				InventoryUtil::switchSlot(bestSlot);
				if (mobEquipment) InventoryUtil::sendMobEquipment(bestSlot);
			}
			if (InventoryUtil::getSelectedSlot() == bestSlot && bestSlot != -1) {
				if (PlayerUtil::tryPlaceBlock(block)) c++;
			}
			if (c >= blocksPerTick) break;
		}
	}
	if (silent && InventoryUtil::getSelectedSlot() != oldSlot && swap) {
		InventoryUtil::switchSlot(oldSlot);
		if (mobEquipment) InventoryUtil::sendMobEquipment(oldSlot);
	}
}

void AutoMapart::onLevelRender() {
	LocalPlayer* localPlayer = Game::getLocalPlayer();
	HitResult* hitResult = localPlayer->level->getHitResult();
	if (hitResult->type == HitResultType::BLOCK && startPos == BlockPos(0, -32000, 0)) {
		const BlockPos& pos = hitResult->blockPos.add(0, 1, 0);
		const AABB& blockAABB = AABB(pos.CastTo<float>(), pos.CastTo<float>().add(1.f));
		MCR::drawBox3dFilled(blockAABB, UIColor(255, 255, 255, 60), UIColor(255, 255, 255, 80));
		if (Game::isRightClickDown()) {
			startPos = pos;
			mapArt.clear();
			image.clear();
		}
	}
	MCR::setColor(mce::Color(1.f, 1.f, 1.f, 1.f));
	MCR::tessellator->begin(VertextFormat::QUAD);
	if (startPos != BlockPos(0, -32000, 0) && !noRender) {
		for (const auto& it : mapArt) {
			if (it.second.has_value()) {
				BlockPos pos = it.first.add(0.f, 1.f, 0.f);
				Vec3<float> p1 = pos.CastTo<float>();
				Vec3<float> p2 = pos.CastTo<float>().add(Vec3<float>(1.f, 0.f, 0.f));
				Vec3<float> p3 = pos.CastTo<float>().add(Vec3<float>(1.f, 0.f, 1.f));
				Vec3<float> p4 = pos.CastTo<float>().add(Vec3<float>(0.f, 0.f, 1.f));

				p1 = p1.sub(MCR::origin);
				p2 = p2.sub(MCR::origin);
				p3 = p3.sub(MCR::origin);
				p4 = p4.sub(MCR::origin);

				const mce::Color color = it.second.value().toMCColor();
				MCR::tessellator->color(color.r, color.g, color.b, color.a);

				MCR::tessellator->vertex(p1.x, p1.y, p1.z);
				MCR::tessellator->vertex(p2.x, p2.y, p2.z);
				MCR::tessellator->vertex(p3.x, p3.y, p3.z);
				MCR::tessellator->vertex(p4.x, p4.y, p4.z);

				MCR::tessellator->vertex(p4.x, p4.y, p4.z);
				MCR::tessellator->vertex(p3.x, p3.y, p3.z);
				MCR::tessellator->vertex(p2.x, p2.y, p2.z);
				MCR::tessellator->vertex(p1.x, p1.y, p1.z);
			}
		}
	}
	MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
}
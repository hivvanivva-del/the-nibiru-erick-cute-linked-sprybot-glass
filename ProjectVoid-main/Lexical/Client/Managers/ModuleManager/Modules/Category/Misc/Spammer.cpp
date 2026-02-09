#include "Spammer.h"
#include "../../../../../../SDK/Game.h"
#include "../../../../../../SDK/MCTextFormat.h"
#include "../../../../../../SDK/Network/Packet/TextPacket.h"
#include "../../../../../Client.h"
#include <cstdlib>

ChatSpammer::ChatSpammer() : Module("Spammer", "Spams chat messages", Category::MISC) {
    // Register settings
    registerSetting(new SliderSetting<float>("Delay", "Delay between messages (seconds)", &delay, 5.0f, 0.5f, 60.0f));
    registerSetting(new BoolSetting("Random Order", "Send messages in random order", &randomOrder, false));
    registerSetting(new EnumSetting("Mode", "Type of messages to send", { "Normal", "Promotional", "Toxic", "Custom" }, &messageMode, 0));

    // Initialize message banks
    normalMessages = {
        "Hello everyone!",
        "Nice weather today!",
        "How's it going?",
        "GG everyone!",
        "Good game!"
    };

    promoMessages = {
        "Check out Void client!",
        "Void client best!",
        "Get Void today!",
        "This spam is brought to you by Void",
        "Void - the best utility mod!"
    };

    toxicMessages = {
    "The bot has been customized by DESTROYER8829. contact me on discord (same name) for custom messages to be spammed",
">> JOIN NIBIRU TODAY https://discord.gg/AcBfZsE7JU",
"World Invason on Top !",
">> BOT POWERED BY VOID CLIENT<<",
"Honey never spoils."
"Bananas are berries, but strawberries are not."
"Octopuses have three hearts."
"A day on Venus is longer than a year on Venus."
"The Eiffel Tower can grow more than six inches in summer."
"Your brain uses about 20procent of your body's oxygen."
"Sharks existed before trees."
"There are more stars in the universe than grains of sand on Earth."
"Butterflies can taste with their feet."
"The shortest war in history lasted about 38 minutes."
"Water can exist in all three states at once under the right conditions."
"Humans share 60procent of their DNA with bananas."
"Wombat poop is cube-shaped."
"The heart of a blue whale is the size of a small car."
"Cats cannot taste sweetness."
"Scotlands national animal is the unicorn."
"Koalas sleep up to 22 hours a day."
"A group of crows is called a murder."
"Hot water can freeze faster than cold water under certain conditions."
"Sloths can hold their breath longer than dolphins."
"Jellyfish have existed for more than 500 million years."
"The human nose can detect over a trillion scents."
"Lightning is hotter than the surface of the sun."
"An ostrichs eye is bigger than its brain."
"Sea otters hold hands while sleeping."
"Polar bear fur is actually transparent."
"The moon has moonquakes."
"Bees can recognize human faces."
"A snail can sleep for three years."
"Humans glow in the dark, but the light is too weak to see."
"Tomatoes were once considered poisonous in Europe."
"Goosebumps are a leftover survival trait from when humans had more hair."
"A day on Mars is only 37 minutes longer than a day on Earth."
"The Great Wall of China is not visible from space with the naked eye."
"Dolphins have names for each other."
"Tigers have striped skin, not just striped fur."
"Spiders can regrow lost limbs."
"Peanuts are not nuts; they are legumes."
"A hummingbirds heart beats more than 1,200 times per minute."
"Cows have best friends."
"Earth is the only planet not named after a god."
"Mosquitoes are the deadliest animals on Earth."
"The speed of a sneeze can exceed 100 miles per hour."
"Giraffes have the same number of neck vertebrae as humans."
"Butterflies were originally called flutterbys."
"Sharks do not get cancer as often as other animals.",
"Turtles can breathe through their butts.",
"Olympic gold medals are mostly silver.",
"There are more fake flamingos in the world than real ones.",
"Some fish can walk on land.",
"A cloud can weigh more than a million pounds.",
"Humans are the only animals with chins.",
"Rats laugh when tickled.",
"There is enough DNA in the human body to stretch from Earth to Pluto and back.",
"Saturn could float in water because it is mostly gas.",
"Penguins propose to each other with stones.",
"Blue eyes are actually colorless and reflect light.",
"Pineapples take up to two years to grow.",
"A crocodile cannot stick its tongue out.",
"There are more atoms in a drop of water than stars in the universe.",
"The longest English word has 189,819 letters.",
"Humans and giraffes both have seven neck vertebrae.",
"Some metals are so reactive they explode when exposed to water.",
"Avocados are berries.",
"You can hear a blue whales heartbeat from two miles away.",
"Earths inner core is as hot as the suns surface.",
"Kangaroos cannot walk backward.",
"One million seconds is about 11.5 days; one billion seconds is about 31.7 years.",
"The average human will walk the equivalent of five times around the world in their lifetime.",
"Snow can be different colors, including pink and green.",
"The longest recorded flight of a chicken is 13 seconds.",
"Venus is the hottest planet in the solar system.",
"The human stomach gets a new lining every few days.",
"Squirrels forget where they hide about 75procent of their nuts.",
"Starfish do not have brains.",
"Humans shed about 600,000 particles of skin every hour.",
"An ant can lift 50 times its own body weight.",
"There are over 200 dead bodies on Mount Everest.",
"Coca-Cola was originally green.",
"The first oranges were actually green."
"Dogs can smell time.",
"Horses cannot vomit.",
"Pigeons can recognize themselves in a mirror.",
"Octopuses have blue blood.",
"The moon is moving away from Earth each year.",
"Grasshoppers have ears on their stomachs.",
"There are more trees on Earth than stars in the Milky Way.",
"Humans are the only animals that blush.",
"The loudest sound produced by an animal is made by the sperm whale.",
"The shortest commercial flight lasts under two minutes.",
"Some turtles can live for more than 150 years.",
"Apples float because they are 25procent air.",
"Gorillas can catch human colds.",
"The worlds largest desert is Antarctica.",
"The average cloud moves at about 30 miles per hour.",
"Octopuses can taste with their arms.",
"There are more chickens on Earth than people."
"The longest hiccup attack lasted 68 years.",
"Humans are the only species known to use fire intentionally.",
"A single strand of spaghetti is called a spaghetto.",
">>> IF YOU SEE THIS THE BOT HAS RAN OUT OF TEXTS. CONTACT DESTROYER8829 ON DISCORD TO ADD CUSTOM TEXTS! initiating re-narating",
    };
}

void ChatSpammer::onNormalTick(LocalPlayer* actor) {
    elapsedTime += 0.05f;

    if (elapsedTime >= delay) {
        elapsedTime = 0.f;
        sendNextMessage();
    }
}

void ChatSpammer::sendNextMessage() {
    std::vector<std::string>* currentBank = &normalMessages;

    switch (messageMode) {
    case PROMOTIONAL: currentBank = &promoMessages; break;
    case TOXIC: currentBank = &toxicMessages; break;
    case CUSTOM: currentBank = &customMessages; break;
    }

    if (currentBank->empty()) return;

    if (randomOrder) {
        currentIndex = rand() % currentBank->size();
    }
    else {
        currentIndex = (currentIndex + 1) % currentBank->size();
    }

    PlayerUtil::SendTextMessage(currentBank->at(currentIndex));
}

void ChatSpammer::addCustomMessage(const std::string& msg) {
    customMessages.push_back(msg);
}

void ChatSpammer::clearCustomMessages() {
    customMessages.clear();
}
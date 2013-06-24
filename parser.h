
#define SITE_CONTENT_START "Tagesübersicht"
#define SITE_TABLE_START "<div class=\"mensa_day\">"
#define SITE_TOPIC_LINE "mensa_day_title"
#define SITE_TOPIC_LINE_FULL "<th class=\"mensa_day_title\" colspan=\"3\">"

#define SITE_PRICES_LINE "mensa_day_speise_preis"
#define SITE_PRICES_LENGTH 24
#define MAX_MALLOC_PRICES_STRING 9

#define MEAL_GRUEN  "gruen"
#define MEAL_ORANGE "orange"
#define MEAL_ROT    "rot"

#define NUM_MEAL_GRUEN 1
#define NUM_MEAL_ORANGE 2
#define NUM_MEAL_ROT 3

struct mealTopic {
    char * description;
    struct mealTopic * nextTopic;
    struct mealListItem * mealList;
    long positionInFile;
};

struct mealListItem {
    struct mealListItem * nextItem;
    int color;
    char * description;
    char * priceStudent;
    char * priceWorker;
    char * priceForeigner;
};

int parsePlan(char * mensaURL, int pNextDay, int pColored, int pPrices[3]);


#define SITE_CONTENT_START "Tagesübersicht"
#define SITE_TITLE_START "<span class=\"bold\">"
#define SITE_TABLE_START "<div class=\"container-fluid splGroupWrapper\">"
#define SITE_MEAL_LINE "splMeal"
#define SITE_TOPIC_LINE "splGroup"
#define SITE_TOPIC_LINE_FULL "<span class=\"bold\">"
#define SITE_TABLE_END "<div id=\"legende\">"

#define SITE_AMPEL "/ampel"
#define SITE_PRICES_NEXT_LINE "col-xs-6 col-md-3 text-right"
#define SITE_PRICES_LINE "&euro;"
#define SITE_PRICES_PARSING "&euro; %[0-9,]/%[0-9,]/%[0-9,]"
#define MAX_MALLOC_PRICES_STRING 9

#define MEAL_GRUEN  "gruen"
#define MEAL_ORANGE "gelb"
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

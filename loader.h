#define PLAN_URL "https://www.stw.berlin/xhr/speiseplan-und-standortdaten.html"
#define PLAN_MENSA_ID "resources_id"
#define PLAN_MENSA_ID_STRLEN 20
#define PLAN_DATE "date="
#define PLAN_DATE_STRLEN 10

#define PLAN_DAY_NAVIGATION "https://www.stw.berlin/xhr/loadTagesReiter.html"
#define PLAN_DAY_NEXTDAY "spltag"
#define PLAN_DAY_VALUE "loadSpeiseplanWochentag('"

char * getValueNextDay(int daysInFuture);
int downloadPage(char * pageURL, char * postData, FILE * filename);

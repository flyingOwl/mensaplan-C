
#define CONFIG_FILE "/.config/mensa.conf\0"

#define LIST_URL "https://www.studentenwerk-berlin.de/mensen/speiseplan/index.html"
#define LIST_URL_LINK "https://www.studentenwerk-berlin.de/mensen/speiseplan/"
#define MENSA_LIST_START "<div id=\"content\">"

int initializer();
char * getConfigPath();

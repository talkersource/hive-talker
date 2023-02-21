
/* ignore bits */
#define IGN_EXIST
#define IGN_PUBLIC
#define IGN_PRIVATE
#define IGN_COLOUR
#define IGN_SPAM
#define IGN_SOCIALS
#define IGN_GAMES

/* flag bits */
#define LIST_INFORM
#define LIST_FRIEND
#define LIST_PREFER

typedef struct list_s
{
     int userid;

     int ignore;

     int flags;

     struct udlist_s *next;
} list_t;

#define MAX_GROUP_NAME 20
#define MAX_GROUP_TAG 10
#define MAX_GROUP_DESC 50

/* group flags */       
#define GROUP_PUBLIC  ( 1 << 0 )
#define GROUP_PRIVATE ( 1 << 1 )

/* the group structure */

typedef struct group_s
{
     char *name;        /* unique group name                          */
     char *tag;         /* the group tag                              */
     char *desc;        /* group description for the list             */
     char  colour;      /* default group colour                       */
     char  letter;      /* the default letter for the groups commands */

     int   flags;

     int   usage_count;   /* incremented each time the group is used  */
     int   last_activity; /* time since the last usage of the group   */
              
     int   creator_id;    /* the user id of the user that created the group */

     ulist_t *users;      /* a list of users of the group */

     /* for private groups only */

//     int  *owners;     /* list of group owners ( zero terminated )  */
//     int  *invited;    /* list of invited userids                     */ 
                                

     struct group_s *next;
} group_t;

typedef struct udgroups_s
{
     group_t *c_default;  /* the users default group */

     group_t *new;        /* pointer to a new group if a user is working on one */
} udgroups_t;



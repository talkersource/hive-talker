
typedef struct history_s
{
     int size;
     
     char **entries;
     char **latest;

} history_t;


extern history_t *history_new( int size );
extern void history_free( history_t *h );
extern char *history_get( history_t *h, int index );
extern void history_set( history_t *h, char *str );

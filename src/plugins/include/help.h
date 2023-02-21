typedef struct help_subject_s
{              
     char *name;
     int index; 

     struct help_subject_s *next;
} help_subject_t;

typedef struct help_entry_s
{
     string_t *raw_text;

     struct help_entry_s *next;
} help_entry_t;

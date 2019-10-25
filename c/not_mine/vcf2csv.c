#include <stdio.h>     /* printf, FILENAME_MAX  */
#include <unistd.h>
#include <string.h>
#include <stdlib.h>    /* exit                  */
#include <sys/types.h>
#include <getopt.h>
#include <assert.h>    /* assertion output      */
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "vcf2csv.h"


char msg[MAX_BUF];
char *program_name;
char *input_file_name;
char *output_file_name;

char dropvalues[MAX_LINE+1];


/* first element of vcard types list */
vcard *vcAnchor = NULL;

/* first element of drop value list  */
drop  *dropAnchor = NULL;

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/******************************************************************************/
void xExit ( FILE *outStream, int returnCode, char *msg )
{
  fprintf ( outStream, "%s", msg );
  if ( returnCode != 0 )
    exit ( returnCode );
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void appendDropWord ( char *pt )
{
  drop
    *hp1,
    *lastDrop,
    *newDrop;

  lastDrop = NULL;
  newDrop  = NULL;

  /* size of drop word is limited to MAX_DROP_WORD                            */
  if (strlen(pt) > MAX_DROP_WORD)
    {
      fprintf ( stderr, "WARNING: ignoring drop word, too large: '%s'\n", pt);
      return;
    }


  /****************************************************************************/
  /* search for existing drop key                                             */
  /* if exists, there is no need to add the actual one, so return             */
  /****************************************************************************/
  for ( hp1=dropAnchor; hp1!=NULL; hp1=hp1->next )
    {
      /* store last used drop struct                                          */
      /* if list is parsed completely and the key is not found, a new one has */
      /* to be attachted to the last one as *next element                     */
      lastDrop = hp1;
      if ( strcmp(hp1->key, pt) == 0 )
	{
	  /* if key exists do nothing                                         */
	  return;
	}
    }

  /****************************************************************************/
  /* key not found, append                                                    */
  /****************************************************************************/

  /* request memory for next drop struct, on error exit programm              */
  if ( (newDrop = (drop *) malloc(sizeof(drop))) == NULL )
      xExit ( stderr, EXIT_FAILURE, "out of memory, can't create drop element");

  /* store drop value in key                                                  */
  strcpy ( newDrop->key, pt );

  /****************************************************************************/
  /* is this the first drop struct, or append to list?                        */
  /****************************************************************************/
  if (dropAnchor == NULL)
    {
      dropAnchor = newDrop;
    }
  else
    {
      lastDrop->next = newDrop;
    }  
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void dumpDropValues()
{
  drop
    *hp1;

  for ( hp1=dropAnchor; hp1!=NULL; hp1=hp1->next )
    {
      fprintf ( stderr, "'%s'\n", hp1->key);
    }
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void splitDropValues(char *dropString)
{
  char *pt;
  
  pt = strtok ( dropString, ",");
  while (pt != NULL)
    {
      appendDropWord(pt);
      pt = strtok ( NULL, "," );
    }
}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/******************************************************************************/
static void printHelp()
{
  printf ("\nusage: %s [-d types][-hvw] -i <vcard input file> \n", program_name);
  printf ("\n");
  
  printf ("%s converts a vcard file to a CSV list or simple HTML page.\n"
	  , program_name );
  printf ("The input file has to be a valid vcard file.\n");
  printf ("The output is written to stdout.\n");
  printf ("\n");
  printf ("command line options:\n");
  printf (" -d : comma separated list of a substring of a type to drop\n");
  printf ("      these types will not apper in convertet data\n");
  printf ("      the drop value is handled case sensitive\n");
  printf ("      On windows only use double quotes for drop values!\n");
  printf ("      examples:\n");
  printf ("        only drop type VERSION:\n");
  printf ("          -d \"VERSION\"\n");
  printf ("        drop VERSION, all addresses and all photos:\n");
  printf ("          -d \"VERSION,ADR,PHOTO\"\n");
  printf ("        drop all adresses, X-enries and telephone numbers:\n");
  printf ("          -d \"ADR;,X-,TEL;\"\n");
  printf (" -i : input  file name (required)\n");
  printf (" -v : print version\n");
  printf (" -w : web output: generate simple html page instead of CSV\n");
  printf (" -h : this help\n");
  printf ("\n");
  printf ("for more details please visit: http://vcf2csv.sourceforge.net\n");
  printf ("(c) Christoph Sandhaus, 2008\n");
  printf ("License: GPL\n");
  printf ("\n");
}


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/******************************************************************************/
static int parse_opts(int argc, char **argv)
{

  /****************************************************************************/
  /* required for getopt                                                      */  
  /****************************************************************************/
  int opt;

  int opt_unknown    = 0;
  int opt_help       = 0;
  int opt_input      = 0;
  int opt_version    = 0;
  int opt_format     = 0;
  int opt_no_opt     = 1;
  int opt_dropvalues = 0;

  opterr = 0;

  /****************************************************************************/
  /* parse options                                                            */  
  /****************************************************************************/
  while ((opt = getopt(argc, argv, "d:hi:vw")) != EOF)
    {
      switch (opt)
	{

	case 'd':
	  opt_dropvalues = 1;
	  opt_no_opt     = 0;
	  (void) strncpy (dropvalues, optarg, sizeof(dropvalues));
	  splitDropValues(dropvalues);
	  break;

	case 'i':
	  if (strlen(optarg) > MAX_FN-5)
	    {
	      sprintf ( msg, "input file name is too long (max %d characters)\n", (int)MAX_FN-5 );
	      xExit   ( stderr, EXIT_FAILURE, msg );
	    }

	  opt_input   = 1;
	  opt_no_opt  = 0;
	  strcpy(input_file_name, optarg);
	  break;

	case 'h':
	  opt_help    = 1;
	  opt_no_opt  = 0;
	  break;

	case 'v':
	  opt_version = 1;
	  opt_no_opt  = 0;
	  break;

	case 'w':
	  opt_format  = 1;
	  opt_no_opt  = 0;
	  break;

	case '?':
	  opt_unknown = 1;
	  fprintf ( stderr, "%s: invalid option: %c\n", program_name, optopt );
	  break;
	}
    }

  if (opt_version)
    {
      printf ( "%s %s\n", program_name, VERSION );
      exit   ( EXIT_SUCCESS );
    }

  if (opt_help)
    {
      printHelp();
      exit    ( EXIT_SUCCESS );
    }

  if (opt_unknown || opt_no_opt)
    {
      fprintf ( stderr, "type: `%s -h' for more information\n", program_name );
      exit    ( EXIT_FAILURE );
    }

  if (opt_format == 0)
    return OUT_CSV;
  else
    return OUT_WEB;

}


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/******************************************************************************/
FILE* xopen (char *fName)
{
  FILE* fp;
  
  if ((fp = fopen ( fName, "r" )) == NULL)
    {
      perror ( "error opening input file: " );
      exit   ( EXIT_FAILURE );
    }

  return fp;
}


/******************************************************************************/
/* types are stored in chan list, vcAnchor is first element                   */
/* - search in list, if element already exists, append if doesn't             */
/*                                                                            */
/* parameter                                                                  */
/* - pt: string containing the type to be included                            */
/******************************************************************************/
void appendVcardType (char *pt)
{
  vcard
    *hp1,
    *lastVcard,
    *newVcard;

  drop
    *hpDrop;

  lastVcard = NULL;
  newVcard  = NULL;



  /****************************************************************************/
  /* if key exists in drop list, key is ignored                               */
  /****************************************************************************/
  for ( hpDrop = dropAnchor; hpDrop!=NULL; hpDrop=hpDrop->next)
    {
      if (strstr(pt, hpDrop->key) != NULL)
	{
	  return;
	}
    }

  /****************************************************************************/
  /* search for existing type                                                 */
  /****************************************************************************/
  for ( hp1=vcAnchor; hp1!=NULL; hp1=hp1->next )
    {
      /* remember last EXISTING vcard entry */
      lastVcard = hp1;
      if ( strcmp(pt, hp1->key) == 0 )
	{
	  /* if key exists do nothing                                         */
	  return;
	}
    }

  /****************************************************************************/
  /* key not found, append                                                    */
  /****************************************************************************/

  /* enough memory?                                                           */
  if ( (newVcard = (vcard *) malloc(sizeof(vcard))) == NULL )
      xExit ( stderr, EXIT_FAILURE, "out of memory" );

  strcpy ( newVcard->key, pt );

  /****************************************************************************/
  /* first element ?                                                          */
  /****************************************************************************/
  if (vcAnchor == NULL)
    {
      vcAnchor = newVcard;
    }
  else
    {
      lastVcard->next = newVcard;
    }
}


/******************************************************************************/
/* print list of keys to stdout                                               */
/******************************************************************************/
void printKeys()
{
  vcard
    *hp;
  
  int
    keyCount = 0;

  printf ( "# list of registered keys:\n" );

  for ( hp=vcAnchor; hp!=NULL; hp=hp->next, keyCount++ )
    printf( "#   %s\n", hp->key );
  
  printf ( "#     key count: %d\n", keyCount );

}



/******************************************************************************/
/*                                                                            */
/*                                                                            */
/******************************************************************************/
int resetVcardList ( const unsigned int mode )
{
  vcard
    *hp;

  
  
  if (mode & RST_VALUE)
    {
      for ( hp=vcAnchor; hp!=NULL; hp=hp->next )
	{
	  strcpy (hp -> val, "");
	}
    }
	
  if (mode & RST_TYPE)
    {
      vcAnchor = NULL;
    }
	
  return ( EXIT_SUCCESS );
}


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/******************************************************************************/



int getRow ( FILE* fp, char *row )
{
  /****************************************************************************/
  /* content of row, including line folded lines                              */
  /****************************************************************************/
  char ptrLine[MAX_LINE+1];

  /****************************************************************************/
  /* line preread to detect linefolding                                       */
  /****************************************************************************/
  char ptrLinePre[MAX_LINE+1];

  /****************************************************************************/
  /* mark return position, if line preseek doesnt detect linefolding          */
  /****************************************************************************/
  fpos_t pos;
  char *c;


  /* read line                                                                */
  if ( fgets(ptrLine, MAX_LINE, fp ) == NULL )
    return ( -1 );

  if (strlen(ptrLine) >=1) {
    size_t p=strlen(ptrLine);

    /* detect end of line and terminate                                       */
    /* replace '\n' with '\0'                                                 */
    ptrLine[p] = '\0';

    /* Windows: detect 'x0d' and terminate */
    if ((c = strchr(ptrLine, '\x0d')) != NULL ) {
      *c = 0;
    }

    /* Windows: detect 'x0d' and terminate */
    if ((c = strchr(ptrLine, '\x0a')) != NULL ) {
      *c = 0;
    }

  }

  /* mark current file position                                               */
  /* return to this position if not linefolding is detected (preread)         */
  if ( fgetpos(fp, &pos) != 0 )
    xExit ( stderr, EXIT_FAILURE, "ERROR reading current file position\n" );
  
  /* read next line                                                           */
  /* - if first char of next line is whitespace => linefolding                */
  /* - no linefolding: return file pointer to previous position               */
  while ( fgets(ptrLinePre, MAX_LINE, fp) != NULL )
    {
      /* detect whitespace on first position in line                          */
      if ( ptrLinePre[0] != ' ' && ptrLinePre[0] != '\t' )
	{
	  /* no linefolding, return to curren position and stop prereading    */
	  if ( fsetpos(fp, &pos) != 0 )
	    xExit ( stderr, EXIT_FAILURE, "ERROR setting file position\n" );

	  break;
	}

      /* seems to be linefolding                                              */

      /* remove CRLF from end of line                                         */
      if (strlen(ptrLinePre)>=2)
	ptrLinePre[strlen(ptrLinePre)-2] = '\0';

      /* mark current file position                                           */
      if ( fgetpos(fp, &pos) != 0 )
	xExit ( stderr, EXIT_FAILURE, "ERROR reading current file position\n" );

      /* ptrPreRead contains whitespace as first element => ignore            */
      /* test for minimum length                                              */
      if (strlen(ptrLinePre) >0)
	strcat ( ptrLine, ptrLinePre+1 );
    }

  strncpy (row, ptrLine, MAX_LINE );
  
  return 0;
  
}


/******************************************************************************/
/******************************************************************************/
int isDropValue(char *key)
{
  return (EXIT_SUCCESS);
}
/******************************************************************************/
/* - parse each vcard                                                         */
/* - strip type from each row                                                 */
/* - add type to type list (call appendVcardType())                           */
/*                                                                            */
/* parameter                                                                  */
/* - fp: file pointer to input file, position is moved by reading             */
/******************************************************************************/
void readTypes ( FILE *fp )
{
  char  *pt;

  /****************************************************************************/
  /* full row, including folded lines                                         */
  /****************************************************************************/
  char ptrLine[MAX_LINE+1];

  /****************************************************************************/
  /* linecount for input file, including empty lines                          */
  /****************************************************************************/
  int lineCount = 0;

  /****************************************************************************/
  /* vcard counter                                                            */
  /****************************************************************************/
  int vcardCount = 0;

  /****************************************************************************/
  /* countVcardBeginEnd:                                                      */
  /* - incremented by one if vcard begins                                     */
  /* - decremented by one if vcard ends                                       */
  /****************************************************************************/
  int countVcardBeginEnd = 0;


  /****************************************************************************/
  /* ensure input file is opened                                              */
  /****************************************************************************/
  if (fp == NULL)
    xExit ( stderr,
	    EXIT_FAILURE,
	    "invalid file descriptor, can't read input file\n" );


  /****************************************************************************/
  /* - while getRow obtains a row from file:                                  */
  /* - count each line, empty lines included                                  */
  /* - ignore contents of empty lines                                         */
  /* - detect begin/end of vcard                                              */
  /* - if data line, colon must be included, otherwise: error                 */
  /* - if seems to be data line, call appendVcardType to strip type           */
  /****************************************************************************/
  while ( getRow(fp, ptrLine) == 0 )
    {
      /* count lines, empty lines included                                    */
      lineCount++;

      /* ignore empty lines                                                   */
      if ( strlen(ptrLine) <= 0 )
	continue;

      /*
      printf ( "%.5d (open vcards: %d): '%s'\n",
	       lineCount,
	       countVcardBeginEnd,
	       ptrLine );
      */

      /* detect start of vcard */
      if ( strcmp(ptrLine, "BEGIN:VCARD") == 0)
	{
	  vcardCount++;
	  countVcardBeginEnd++;
	  continue;
	}
      
      /* detect end of vcard */
      if ( strcmp(ptrLine, "END:VCARD") == 0)
	{
	  countVcardBeginEnd--;
	  continue;
	}
      
      /* at this point ptrLine conains vcard entry => add type to struct      */
      /* line MUST include colon                                              */
      /* countVcardBeginEnd equals to 1, otherwise start/stop detection error */
      if ( countVcardBeginEnd != 1 )
	xExit ( stderr,
		EXIT_FAILURE,
		"ERROR vcard error? malicous line in vcard?" );
      
      /* search for colon                                                     */
	if ( (pt = strchr(ptrLine, ':')) == NULL )
	{
	  /* error: no colon. stop processing                                 */
	  fprintf (stderr, "ERROR: vcard entry wihtout colon: '%s'\n", ptrLine);
	  continue;
	}

      /* replace colon by line delimitter to add type to type list            */
      *pt = 0;
      if (!isDropValue(ptrLine))
	appendVcardType ( ptrLine );
    }
  
  /* printKeys(); */
  /* printf ( "vcards:\t\t\t%d\nbegin/end count:\t%d\n", vcardCount, countVcardBeginEnd); */
}


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/******************************************************************************/
void storeKeyVal ( char *key, char *val )
{
  vcard
    *hp1;

  /****************************************************************************/
  /* search for existing type                                                 */
  /****************************************************************************/
  for ( hp1=vcAnchor; hp1!=NULL; hp1=hp1->next )
    {
      if ( strcmp(hp1->key, key) == 0 )
	{
	  /* store value                                                      */
	  strcpy (hp1->val, val);
	  return;
	}
    }
}


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/******************************************************************************/
void printRow (const int format)
{
  vcard
    *hp;

  if (format & OUT_WEB)
    printf ("<tr>\n");
  
  for ( hp=vcAnchor; hp!=NULL; hp=hp->next )
    if (format & OUT_CSV)
      printf ( "%s\t", hp->val );
    else if (format & OUT_WEB)
      printf ( "  <td>%s</td>\n", hp->val );

  if (format & OUT_WEB)
    printf ("</tr>\n");
  else if (format & OUT_CSV)
    printf("\n");
  
}


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/******************************************************************************/
void printFooter (const int format)
{
  if (format & OUT_WEB)
    {
      printf ("</table></body></html>\n");
    }
}


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/******************************************************************************/
void printHeader (const int format)
{
  vcard
    *hp;
	
  char *p;
  char win_name[MAX_BUF];
  
  /* In case the prog name contains a dot, it may be compiled for windows */
  /* Strip '.exe' extension from name */
  
  if (format & OUT_WEB)
    {
	
	  if ((p=strchr(program_name, '.')) != NULL) {
		*p = 0;
	  }
      printf ("<html>\n <head>\n  <title>%s: vCard2HTML view</title>\n" , program_name);
      printf ("  <link rel=\"stylesheet\" type=\"text/css\" href=\"%s.css\">\n" , program_name );
      printf (" </head>\n");
      printf ("<body>\n\n");
      printf ("<table>\n");
      printf ("<tr>\n");
    }

  for ( hp=vcAnchor; hp!=NULL; hp=hp->next )
    {
      if (format & OUT_CSV)
	printf( "%s\t", hp->key );
      
      else if (format & OUT_WEB)
	{
	  printf ("  <th>%s</th>\n", hp->key );
	}
    }

  if (format & OUT_WEB)
    {
      printf ("</tr>\n");
    }
  else if (format & OUT_CSV)
    printf("\n");
  
}


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/******************************************************************************/
void removeTabChar ( char *text )
{
  char
    newText[MAX_LINE+1],
    *pt1,
    *pt2;

  while ( (pt1 = strchr( text, '\t')) != NULL )
    {
      /* replace TAB by string terminator */
      *pt1 = 0;
  
      /* copy until occurence of TAB (pt1) */
      strcpy ( newText, text);
      
      /* append string "\t" */
      strcat  ( newText, "\\t" );
      
      /* append rest of pt1 */
      strcat  ( newText, pt1+1 );
      
      /* copy result to origin */
      strcpy  (text, newText);
    }
}


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/******************************************************************************/
void createOutput ( FILE * fp, const int format )
{
  /****************************************************************************/
  /* full row, including folded lines                                         */
  /****************************************************************************/
  char ptrLine[MAX_LINE+1];

  char
    ptHelp[MAX_LINE+1],
    *pt2,
    *pt;
  char
    val[MAX_LINE+1],
    type[MAX_LINE+1];

  /****************************************************************************/
  /* countVcardBeginEnd:                                                      */
  /* - incremented by one if vcard begins                                     */
  /* - decremented by one if vcard ends                                       */
  /****************************************************************************/
  int countVcardBeginEnd = 0;
  
  printHeader(format);

  while ( getRow(fp, ptrLine) == 0 )
    {
      /* ignore empty lines                                                   */
      if ( strlen(ptrLine) <= 0 )
	continue;      

      /* detect start of vcard */
      if ( strcmp(ptrLine, "BEGIN:VCARD") == 0)
	{
	  countVcardBeginEnd++;
	  continue;
	}
      
      /* detect end of vcard */
      if ( strcmp(ptrLine, "END:VCARD") == 0)
	{
	  countVcardBeginEnd--;
	  printRow(format);
	  /* reset vcard values                                               */
	  (void) resetVcardList ( RST_VALUE );

	  continue;
	}
      
      /* at this point ptrLine conains vcard entry => add type to struct      */
      /* line MUST include colon                                              */
      /* countVcardBeginEnd equals to 1, otherwise start/stop detection error */
      if ( countVcardBeginEnd != 1 )
	{
	  fprintf ( stderr, "ERROR: entry: '%s'\n", ptrLine );
	  xExit ( stderr,
		EXIT_FAILURE,
		"ERROR vcard error? malicous line in vcard?" );
	}
      
      /* search for colon                                                     */
      if ( (pt = strchr(ptrLine, ':')) == NULL )
	{
	  fprintf ( stderr, "ERROR: entry: '%s'\n", ptrLine );
	  /* error: no colon. stop processing                                 */
	  fprintf (stderr, "ERROR: vcard entry wihtout colon: '%s'\n", ptrLine);
	  continue;
	}

      /* replace colon by line delimitter to add type to type list            */
      *pt = 0;
      /* copy key */
      strcpy (type, ptrLine);

      /* step forward to value */
      pt++;

      /* substitute TAB in value, copy value */
      removeTabChar(pt);


      strcpy (val, pt);

      /*printf ("TYPE: '%s' \t---VAL: '%s'\n", type, val); */
      storeKeyVal(type, val);

    }
  
  printFooter( format );
  
}


/******************************************************************************/
/*                                                                            */
/*                                                                            */
/******************************************************************************/
int main(int argc, char **argv)
{
  FILE *fpInput;

  /* standard output format is CSV, may be changed to OUT_WEB                 */
  int
    fmtOutput = OUT_CSV;

  input_file_name   = (char *) malloc (MAX_FN); 
  output_file_name  = (char *) malloc (MAX_FN); 

  if((program_name  = strrchr(argv[0], '/')) == NULL)
    program_name    = argv[0];
  else
    ++program_name; /* move pointer to char after '/' */

  assert (program_name != NULL);

  fmtOutput = parse_opts(argc, argv);

  fpInput = xopen(input_file_name);

  /* parse input file, capture all used types */
  (void) readTypes ( fpInput );
  
  /* return input file pointer */
  (void) rewind    ( fpInput );

  /* (void) resetVcardList ( RST_TYPE );             */
  /* (void) resetVcardList ( RST_VALUE | RST_TYPE ); */

  (void) createOutput ( fpInput, fmtOutput );

  exit(0);
}

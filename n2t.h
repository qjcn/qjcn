#ifndef n2t_h
#define n2t_h

#include <QtCore/QRegExp>
#include <QtCore/QList>
#include <QtCore/QDate>
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace n2t
{

static char one_array[10][11] = 
{
	"ZERO ",
	"ONE ",
	"TWO ",
	"THREE ",
	"FOUR ",
	"FIVE ",
	"SIX ",
	"SEVEN ",
	"EIGHT ",
	"NINE "
};

static char ten_array[10][11] = 
{
	"TEN ",
	"ELEVEN ",
	"TWELVE ",
	"THIRTEEN ",
	"FOURTEEN ", 
	"FIFTEEN ",
	"SIXTEEN ",
	"SEVENTEEN ",
	"EIGHTEEN ",
	"NINETEEN "
};

static char twenty_array[10][11] = 
{
	" ",
	" ",
	"TWENTY ", 
	"THIRTY ", 
	"FORTY ",
	"FIFTY ",
	"SIXTY ",
	"SEVENTY ",
	"EIGHTY ",
	"NINETY "
};

static char big_unit_array[22][20] = 
{
	"VIGINTILLION ",
	"NOVEMDECILLION ",
	"OCTODECILLION ",
	"SEPTENDECILLION ",
	"SEXDECILLION ",
	"QUINDECILLION ",
	"QUATTUORDECILLION ",
	"TREDECILLION ",
	"DUODECILLION ",
	"UNDECILLION ",
	"DECILLION ",
	"NONILLION ",
	"OCTILLION ",
	"SEPTILLION ",
	"SEXTILLION ",
	"QUINTILLION ",
	"QUADRILLION ",
	"TRILLION ",
	"BILLION ",
	"MILLION ",
	"THOUSAND ",
	"HUNDRED "
};


static QByteArray  array_dates_cardinal[32] = 
{
	" ",
	"ONE ",
	"TWO ", 
	"THREE ", 
	"FOUR ", 
	"FIVE ",
	"SIX ",
	"SEVEN ",
	"EIGHT ",
	"NINE ",

	"TEN ", 
	"ELEVEN ", 
	"TWELVE ", 
	"THIRTEEN ", 
	"FOURTEEN ", 
	"FIFTEEN ", 
	"SIXTEEN ", 
	"SEVENTEEN ", 
	"EIGHTEEN ", 
	"NINETEEN ",

	"TWENTY ",
	"TWENTY ONE ",
	"TWENTY TWO ",
	"TWENTY THREE ",
	"TWENTY FOUR ",
	"TWENTY FIVE ",
	"TWENTY SIX ",
	"TWENTY SEVEN ",
	"TWENTY EIGHT ",
	"TWENTY NINE ",

	"THIRTY ",
	"THIRTY ONE "
};


static QByteArray  array_dates_ordinal[32] = 
{
	" ",
	"FIRST ",
	"SECOND ", 
	"THIRD ", 
	"FOURTH ", 
	"FIFTH ",
	"SIXTH ",
	"SEVENTH ",
	"EIGHTH ",
	"NINTH ",

	"TENTH ", 
	"ELEVENTH ", 
	"TWELFTH ", 
	"THIRTEENTH ", 
	"FOURTEENTH ", 
	"FIFTEENTH ", 
	"SIXTEENTH ", 
	"SEVENTEENTH ", 
	"EIGHTEENTH ", 
	"NINETEENTH ",

	"TWENTIETH ",
	"TWENTY FIRST ",
	"TWENTY SECOND ",
	"TWENTY THIRD ",
	"TWENTY FOURTH ",
	"TWENTY FIFTH ",
	"TWENTY SIXTH ",
	"TWENTY SEVENTH ",
	"TWENTY EIGHTH ",
	"TWENTY NINTH ",
				
	"THIRTIETH ",
	"THIRTY FIRST "
};

// ---------------------------------------------------------------
inline QByteArray
num2text(const QByteArray & s)
{
	QByteArray ss = s.toUpper();

//std::cout << "num2text " << ss << std::endl;

	QByteArray ret = "";
	
	for (int i = 0; i < ss.size(); ++i)
	{
		char a = ss[i];

/*
std::cout << "  " 
		<< a << " " 
		<< ret << std::endl;
*/

		switch (a)
		{
			case '0':
				ret += " ZERO";
			break;

			case '1':
				ret += " ONE";
			break;

			case '2':
				ret += " TWO";
			break;

			case '3':
				ret += " THREE";
			break;

			case '4':
				ret += " FOUR";
			break;

			case '5':
				ret += " FIVE";
			break;

			case '6':
				ret += " SIX";
			break;

			case '7':
				ret += " SEVEN";
			break;

			case '8':
				ret += " EIGHT";
			break;

			case '9':
				ret += " NINE";
			break;
			
			default:
			break;
		}
	}

//	std::cout << "num2text=" << ret << std::endl; 

	return ret;
}
// ---------------------------------------------------------------
inline void 
_num2text(char * only_number, char * outputString)
{
const int MaxUnit = 21;
char tempString[150][20];
//E.g. 1,234,567,890,123
int i; //for only_number
int j; //for tempString
int rem;
int sizeofNumber = strlen(only_number);
int tflag = 0;

	for(i=0,j=0; i<sizeofNumber; i++) 
	{
		rem = (sizeofNumber-i)%3;
		if(!rem)
			tflag = 0;

		if(only_number[i] != '0') 
		{
			if(rem == 1) //in oneth position
			{
                strcpy(tempString[j++],one_array[only_number[i]-48]);
				tflag = 1;
			}
			else 
			if(rem == 2) 
			{ 
				//in tenth position
                if(only_number[i] == '1')
					strcpy(tempString[j++],ten_array[only_number[++i]-48]);
                else
					strcpy(tempString[j++],twenty_array[only_number[i]-48]);

				tflag = 1;
            }
            else 
			if(rem == 0) 
			{	// in hundredth position
				strcpy(tempString[j++],one_array[only_number[i]-48]);
				strcpy(tempString[j++],big_unit_array[MaxUnit]);
				tflag = 1;
			}
		}

		if(tflag)
		{
			switch(sizeofNumber-i) 
			{
				//select which big_unit_array to be placed behind the number
				case 64: strcpy(tempString[j++],big_unit_array[MaxUnit-21]); break;
				case 61: strcpy(tempString[j++],big_unit_array[MaxUnit-20]); break;
				case 58: strcpy(tempString[j++],big_unit_array[MaxUnit-19]); break;
				case 55: strcpy(tempString[j++],big_unit_array[MaxUnit-18]); break;
				case 52: strcpy(tempString[j++],big_unit_array[MaxUnit-17]); break;
				case 49: strcpy(tempString[j++],big_unit_array[MaxUnit-16]); break;
				case 46: strcpy(tempString[j++],big_unit_array[MaxUnit-15]); break;
				case 43: strcpy(tempString[j++],big_unit_array[MaxUnit-14]); break;
				case 40: strcpy(tempString[j++],big_unit_array[MaxUnit-13]); break;
				case 37: strcpy(tempString[j++],big_unit_array[MaxUnit-12]); break;
				case 34: strcpy(tempString[j++],big_unit_array[MaxUnit-11]); break;
				case 31: strcpy(tempString[j++],big_unit_array[MaxUnit-10]); break;
				case 28: strcpy(tempString[j++],big_unit_array[MaxUnit-9]); break;
				case 25: strcpy(tempString[j++],big_unit_array[MaxUnit-8]); break;
				case 22: strcpy(tempString[j++],big_unit_array[MaxUnit-7]); break;
				case 19: strcpy(tempString[j++],big_unit_array[MaxUnit-6]); break;
				case 16: strcpy(tempString[j++],big_unit_array[MaxUnit-5]); break;
				case 13: strcpy(tempString[j++],big_unit_array[MaxUnit-4]); break;
				case 10: strcpy(tempString[j++],big_unit_array[MaxUnit-3]); break;
				case 7 : strcpy(tempString[j++],big_unit_array[MaxUnit-2]); break;
				case 4 : strcpy(tempString[j++],big_unit_array[MaxUnit-1]); break;
			}
		}
	}

	j--; 

	//to minus an extra increment of j
	//paste text from tempString to outputString

	strcpy(outputString,tempString[0]);
	for(i=1; i<=j; i++)
		strcat(outputString, tempString[i]);
}
// ---------------------------------------------------------------
inline void 
num2text(const QByteArray & input, QByteArray & output)
{
const int MaxUnit = 21;
char tempString[150][20];
//E.g. 1,234,567,890,123
int i; //for only_number
int j; //for tempString
int rem;
int tflag = 0;

output="";
QByteArray workStr = input.simplified();
if (workStr.size()>0 && workStr[0]=='-')
{
	output += " MINUS ";
	workStr = workStr.remove(0,1);
}

if (workStr.size()>0 && workStr[0]=='0')
{
	output += " ZERO ";
	workStr = workStr.remove(0,1);
}


int sizeofNumber = workStr.size();

	if (sizeofNumber == 1) // single digit
	{
		output += num2text(workStr);
		return;
	}

	
	for(i=0,j=0; i< sizeofNumber; ++i) 
	{
		rem = (sizeofNumber - i) % 3;
		if(!rem)
			tflag = 0;

		if( workStr[i] != '0' )
		{
			if(rem == 1) //in oneth position
			{
                strcpy(tempString[j++],one_array[workStr[i] - 48]);
				tflag = 1;
			}
			else 
			if(rem == 2) //in tenth position
			{				
                if(workStr[i] == '1')
					strcpy(tempString[j++],ten_array[workStr[++i]-48]);
                else
					strcpy(tempString[j++],twenty_array[workStr[i]-48]);

				tflag = 1;
            }
            else 
			if(rem == 0) // in hundredth position
			{	
				strcpy(tempString[j++],one_array[workStr[i]-48]);
				strcpy(tempString[j++],big_unit_array[MaxUnit]);
				tflag = 1;
			}
		}


		if(tflag)
		{
			switch(sizeofNumber - i) 
			{
				//select which big_unit_array to be placed behind the number
				case 64: strcpy(tempString[j++],big_unit_array[MaxUnit-21]); break;
				case 61: strcpy(tempString[j++],big_unit_array[MaxUnit-20]); break;
				case 58: strcpy(tempString[j++],big_unit_array[MaxUnit-19]); break;
				case 55: strcpy(tempString[j++],big_unit_array[MaxUnit-18]); break;
				case 52: strcpy(tempString[j++],big_unit_array[MaxUnit-17]); break;
				case 49: strcpy(tempString[j++],big_unit_array[MaxUnit-16]); break;
				case 46: strcpy(tempString[j++],big_unit_array[MaxUnit-15]); break;
				case 43: strcpy(tempString[j++],big_unit_array[MaxUnit-14]); break;
				case 40: strcpy(tempString[j++],big_unit_array[MaxUnit-13]); break;
				case 37: strcpy(tempString[j++],big_unit_array[MaxUnit-12]); break;
				case 34: strcpy(tempString[j++],big_unit_array[MaxUnit-11]); break;
				case 31: strcpy(tempString[j++],big_unit_array[MaxUnit-10]); break;
				case 28: strcpy(tempString[j++],big_unit_array[MaxUnit-9]); break;
				case 25: strcpy(tempString[j++],big_unit_array[MaxUnit-8]); break;
				case 22: strcpy(tempString[j++],big_unit_array[MaxUnit-7]); break;
				case 19: strcpy(tempString[j++],big_unit_array[MaxUnit-6]); break;
				case 16: strcpy(tempString[j++],big_unit_array[MaxUnit-5]); break;
				case 13: strcpy(tempString[j++],big_unit_array[MaxUnit-4]); break;
				case 10: strcpy(tempString[j++],big_unit_array[MaxUnit-3]); break;
				case 7 : strcpy(tempString[j++],big_unit_array[MaxUnit-2]); break;
				case 4 : strcpy(tempString[j++],big_unit_array[MaxUnit-1]); break;
			}
		}
	}

	j--; 

	//to minus an extra increment of j
	//paste text from tempString to outputString

	output += tempString[0];
	for(i=1; i<=j; i++)
		output += tempString[i];
}
// ----------------------------------------------------------
// only up to 99
inline void
num2text_x(int number, QByteArray & res)
{
	const char *digits[] = 	{"","ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE" };
	const char *teen[] = 	{ "TEN", "ELEVEN ", "TWELVE", "THIRTEEN", "FOURTEEN", "FIFTEEN", "SIXTEEN", "SEVENTEEN", "EIGHTEEN", "NINETEEN" };
	const char *big[] = 	{ "", "", "TWENTY ", "THIRTY ", "FOURTY ", "FIFTY ", "SIXTY ", "SEVENTY ", "EIGHTTY ", "NINETY " };

	unsigned char sdigit = number % 10;
	unsigned char fdigit = number / 10;

	QByteArray ret;	
	switch (fdigit)
	{
		case 0: 
			res += digits[ sdigit ];
		break;

		case 1: 
			res += teen[ sdigit ];
		break;

		default:
			res += big[ fdigit];
			res += digits[ sdigit];
		break;
	}

	res += " ";
}
// ---------------------------------------------------------------
inline void 
num2text_date(const QByteArray & input, QByteArray & output, int how)
{
QByteArray workStr = input.simplified();

	int n = input.toInt();
	if (n>=1 && n<=31)
	{
		if (how==1)
			output = array_dates_cardinal[n];
		else
		if (how==2)
			output = array_dates_ordinal[n];
	}
}

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
struct numeral 
{
      long val;
      int  ch;
};

static struct numeral numerals[] = 
{
      {    1L, 'I' },
      {    5L, 'V' },
      {   10L, 'X' },
      {   50L, 'L' },
      {  100L, 'C' },
      {  500L, 'D' },
      { 1000L, 'M' }
};

/*
**  roman2long() - Converts a roman numeral string into a long integer
**
**  Arguments: 1 - Roman numeral string
**
**  Returns: Long value if valid, else -1L
*/

inline long 
roman2long(const char *str)
{
      int i, j, k;
      long retval = 0L;

      if (!str || 0 == *str)
            return -1L;
      for (i = 0, k = -1; str[i]; ++i)
      {
            for (j = 0; j < 7; ++j)
            {
                  if (numerals[j].ch == toupper(str[i]))
                        break;
            }
            if (7 == j)
                  return -1L;
            if (k >= 0 && k < j)
            {
                  retval -= numerals[k].val * 2;
                  retval += numerals[j].val;
            }
            else  retval += numerals[j].val;
            k = j;
      }
      return retval;
}
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------


typedef struct tag_RomanToDecimal 
{
      long  PostValue;
      const char * PostLetter;
      long  PreValue;
      const char * PreLetter;
} R2D;

/*
**  Set PrefixesAreOK = True_ to enable prefix notation (e.g. 4 = "IV")
**  Set PrefixesAreOK = False_ to disable prefix notation (e.g. 4 = "IIII")
*/

static bool PrefixesAreOK = true;

static R2D RomanConvert[] = 
{
        /*
        ** A Roman numeral with a bar over it is
        ** that value multiplied by 1000.
        */
        {1000000L, "<M-bar>", 900000L, "<CM-bar>"},
        {500000L, "<D-bar>", 400000L, "<CD-bar>"},
        {100000L, "<C-bar>", 90000L, "<XC-bar>"},
        {50000L, "<L-bar>", 40000L, "<XL-bar>"},
        {10000L, "<X-bar>", 9000L, "<IX-bar>"},
        {5000L, "<V-bar>", 4000L, "<IV-bar>"},

        {1000L, "M", 900L, "CM"},
        {500L, "D", 400L, "CD"},
        {100L, "C", 90L, "XC"},
        {50L, "L", 40L, "XL"},
        {10L, "X", 9L, "IX"},
        {5L, "V", 4L, "IV"},
        {1L, "I", 1L, "I"}
    };

/*
**  long2roman() - Convert a long integer into roman numerals
**
**  Arguments: 1 - Value to convert
**             2 - Buffer to receive the converted roman numeral string
**             3 - Length of the string buffer
**
**  Returns: Pointer to the buffer, else NULL if error or buffer overflow
*/

inline char *
long2roman(long val, char * buf, size_t buflen)
{
   size_t posn = 0;
   size_t place = 0;

	do
    {
    	while (val >= RomanConvert[place].PostValue)
        {
        	posn += sprintf(&buf[posn], "%s",
                            RomanConvert[place].PostLetter);
            val -= RomanConvert[place].PostValue;
            if (posn >= buflen)
            	return NULL;
        }
        
		if (PrefixesAreOK)
        {
        	if (val >= RomanConvert[place].PreValue)
            {
            	posn += sprintf(&buf[posn], "%s",
                	RomanConvert[place].PreLetter);
                val -= RomanConvert[place].PreValue;
                if (posn >= buflen)
               		return NULL;
            }
		}
        place++;
      } while (val > 0);

      return buf;
}
// ------------------------------------------------------------------------------------
inline QByteArray
d2roman(long val)
{
QByteArray ret;

	size_t posn = 0;
    size_t place = 0;

	do
    {
    	while (val >= RomanConvert[place].PostValue)
        {
			ret += RomanConvert[place].PostLetter;
            val -= RomanConvert[place].PostValue;
        }
        
		if (PrefixesAreOK)
        {
        	if (val >= RomanConvert[place].PreValue)
            {
				ret += RomanConvert[place].PreLetter;			
                val -= RomanConvert[place].PreValue;
            }
		}
        place++;
      } 
	  while (val > 0);

      return ret;
}
// ------------------------------------------------------------------------------------
inline QByteArray
d2roman(const QByteArray & val)
{
	return d2roman(val.toLong());
}
// ------------------------------------------------------------------------------------
inline QString
d2roman(const QString & val)
{
	return d2roman(val.toLong());
}
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------


// ---------------------------------------------------------------
inline QByteArray
dec2roman(int n)
{
QByteArray ret;

	return ret;
}
// ---------------------------------------------------------------
inline int
roman2dec(const QByteArray & s)
{
int ret = 0;
int op = 1;
int pri = 0;
int lastPri = 0;

	QByteArray ss = s.toUpper();

//std::cout << "roman2dec " << ss << std::endl;

	for (int i = ss.size()-1; i >= 0; --i)
	{
		char a = ss[i];
/*
std::cout << "  " 
		<< a << " " 
		<< pri << " " 
		<< lastPri << " " 
		<< op << " " 
		<< ret << std::endl;
*/
		switch (a)
		{
			case 'M':
				pri = 8;

				if (pri < lastPri) op = -1; 
				else if (pri > lastPri) op = 1; 

				ret += 1000 * op;
			break;

			case 'D':
				pri = 7;

				if (pri < lastPri) op = -1; 
				else if (pri > lastPri) op = 1; 

				ret += 500 * op;
			break;

			case 'C':
				pri = 6;

				if (pri < lastPri) op = -1; 
				else if (pri > lastPri) op = 1; 

				ret += 100 * op;
			break;

			case 'L':
				pri = 4;

				if (pri < lastPri) op = -1; 
				else if (pri > lastPri) op = 1; 

				ret += 50 * op;
			break;
			
			case 'X':
				pri = 3;

				if (pri < lastPri) op = -1; 
				else if (pri > lastPri) op = 1; 

				ret += 10 * op;
			break;

			case 'V':
				pri = 2;

				if (pri < lastPri) op = -1; 
				else if (pri > lastPri) op = 1; 

				ret += 5 * op;
			break;

			case 'I':				
				pri = 1;

				if (pri < lastPri) op = -1; 
				else if (pri > lastPri) op = 1; 

				ret += 1 * op;
			break;
			
			default:
			break;
		}
		
		lastPri = pri;
	}

//	std::cout << "The decimal value is " << ret << std::endl; 
	
	return ret;
}
// ---------------------------------------------------------------
inline bool
isValidRoman(const QByteArray & s)
{
	if (s.size() == 0) return false;

	bool ret = true;
	QByteArray ss = s.toUpper();
	
	for (unsigned int i=0; i < ss.size(); ++i)
	{
		char a = ss[i];

		switch (a)
		{
			case 'M':
			case 'D':
			case 'C':
			case 'L':
			case 'X':
			case 'V':
			case 'I':				
			break;
			
			default:
				ret = false;
			break;
		}
	}

//	std::cout << "Valid roman " << ss << " " << ret << std::endl; 
		
	return ret;
}
// ---------------------------------------------------------------
inline bool
isValidPhone(const QByteArray & s)
{
	if (s.size() == 0) return false;

	bool ret = true;
	bool haveDash = false;
	QByteArray ss = s.toUpper();
	
	for (unsigned int i=0; i < ss.size(); ++i)
	{
		char a = ss[i];

		switch (a)
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			break;

			case '-':
				haveDash = true;
			break;
			
			default:  // any other char makes it invalid
				ret = false;
			break;
		}
	}

	ret = ret && haveDash;
//	std::cout << "isValidPhone " << ret << " " << ret << std::endl; 
		
	return ret;
}

// ---------------------------------------------------------------
// coordinate can come in few styles
//
// 1 - 32o02'54.68"N          		Degrees, Minutes and Seconds in decimal with Direction
//
// or
//
// 2 - 35.598955,-105.88623			Latitude,Longitude      - Negative values denote South and West
//
//
// or
//
// 3 - 32o02.6787'N          		Degrees with decimal minutes
//
// or
//
// 4 - 42.8600oN,-112.4292oW		Degrees in decimal notation
//


// let see which style is in use
static QRegExp coord_rx1("([0-9]+)o([0-9]+)'([0-9]+.?[0-9]+)\"([N,S,W,E])");
static QRegExp coord_rx2("([+,-]?[0-9]+.[0-9]+),([+,-]?[0-9]+.[0-9]+)");

static QRegExp coord_rx3("[0-9]+o[0-9]+.?[0-9]+'[N,S,W,E]");
static QRegExp coord_rx4("[0-9]+.?[0-9]o[N,S,W,E]");


inline int
isValidCoordinate(const QByteArray & s)
{
//std::cout << "isValidCoordinate: " << s << std::endl;

	if (s.size() == 0) return 0;

	int ret = 0;
	
	QString str = s;
	
	if (str.contains(coord_rx1))
	{
		ret = 1;
	}
	else	
	if (str.contains(coord_rx2))
	{
		ret = 2;
	}

//std::cout << "isValidCoordinate: " << s << " " << ret << std::endl;

	return ret;
}

// ---------------------------------------------------------------
inline QByteArray
coordinate2text(const QByteArray & s, int c)
{
	QString str = s;

	QByteArray ret = "";
	
	switch(c)
	{
		case 1:
		{
			// get degrees, minutes, seconds and dir
			ret = "COORDINATE ";

			int pos = coord_rx1.indexIn(str);
			QStringList list = coord_rx1.capturedTexts();

//std::cout << "list size: " << 	list.size() << ": " << list << std::endl;

			if (list.size() == 5) // we must have 4 components - 0 is all
			{				
				QByteArray wstr;

				// get degrees
				num2text(list.at(1).toLatin1(), wstr);
				ret += wstr + " DEGREES ";

				// get minutes
				num2text(list.at(2).toLatin1(), wstr);
				ret += wstr + " MINUTES ";

				// get seconds
				if ( list.at(3).contains('.') ) // decimal number
				{
					int dotIdx = list.at(3).indexOf('.');
					QByteArray wholePart = list.at(3).left(dotIdx).toLatin1();
					QByteArray decimalPart = list.at(3).right( list.at(3).size()-dotIdx).toLatin1();

					num2text(wholePart, wstr);
					wstr += " POINT " + num2text(decimalPart);
					ret += wstr + " SECONDS ";
				}
				else
				{
					num2text(list.at(3).toLatin1(), wstr);
					ret += wstr + " SECONDS ";
				}
				
				// direction
				if 	(list.at(4)=="N")
					ret += " NORTH ";
				else
				if 	(list.at(4)=="S")
					ret += " SOUTH ";
				else
				if 	(list.at(4)=="W")
					ret += " WEST ";
				else
				if 	(list.at(4)=="E")
					ret += " EAST ";													
			}	
		}
		break;

		case 2:
		{
			int pos = coord_rx2.indexIn(str);
			QStringList list = coord_rx2.capturedTexts();
			if (list.size() == 3) // we must have 2 components
			{
				QByteArray wstr;

				// latitude longitude
				ret = " LATITUDE ";
				
				if ( list.at(1).contains('.') ) // decimal number
				{
					int dotIdx = list.at(1).indexOf('.');
					QByteArray wholePart = list.at(1).left(dotIdx).toLatin1();
					QByteArray decimalPart = list.at(1).right( list.at(1).size()-dotIdx).toLatin1();

					num2text(wholePart, wstr);
					wstr += " POINT " + num2text(decimalPart);

					ret += wstr;
				}
				else
				{
					num2text(list.at(1).toLatin1(), wstr);
					ret += wstr;
				}

				ret += " LONGITUDE ";

				if ( list.at(2).contains('.') ) // decimal number
				{
					int dotIdx = list.at(2).indexOf('.');
					QByteArray wholePart = list.at(2).left(dotIdx).toLatin1();
					QByteArray decimalPart = list.at(2).right( list.at(2).size()-dotIdx).toLatin1();

					num2text(wholePart, wstr);
					wstr += " POINT " + num2text(decimalPart);
					ret += wstr;
				}
				else
				{
					num2text(list.at(2).toLatin1(), wstr);
					ret += wstr;
				}
			}
		}
		break;
	}

//	std::cout << "coordinate2text=" << ret << std::endl; 

	return ret;
}
// ---------------------------------------------------------------
inline void
enhStr(QByteArray & workStr, 
		bool doNum,
		int	 doDate,  // 1=cardinal, 2=ordinal
		bool doRom,
		bool doPhone,
		bool doCoord,
		QString & dispStr)
{
	if (!doNum && !doDate && !doRom && !doPhone & !doCoord) // do nothing
		return;

			
	// tokenize into words - to find numbers, dates, romans or phone/ss
	// number can be any whole integer
	// dates must be formated as MM/DD/YYYY
	// romans must have MDCLXVI letters only
	// phone num/ss num must in  123-123-1234  123-12-1234
	
	QList<QByteArray> qbal = workStr.split(' ');

	// clear both - they will be rebuild
	workStr = "";
	dispStr = "";

	for (int i=0; i< qbal.size(); ++i)
	{
		QByteArray token = qbal[i].simplified();
		
		QByteArray workToken = token;
		QByteArray dispToken = token;
				
		// test for number
		bool ok;
		if (doNum)
		{
			if ( workToken.contains('.') ) // decimal number
			{
				// separate number into two halves   whole and decimal
				int dotIdx = workToken.indexOf('.');
				QByteArray wholePart = workToken.left(dotIdx);
				QByteArray decimalPart = workToken.right(workToken.size()-dotIdx);

				long ll = wholePart.toLong(&ok);
				if (ok) // good num
				{
					QByteArray wstr;
					num2text(wholePart, wstr);

					wstr += " point " + num2text(decimalPart);
					
					workToken.replace(token, wstr);					// red
					dispToken.replace(token, "<span style=\"color: rgb(255, 0, 0);\">" + token + "</span>");
				}
				
				
			}
			else  // whole number
			{
				long ll = workToken.toLong(&ok);
				if (ok) // good num
				{
					QByteArray wstr;
					num2text(workToken, wstr);

					workToken.replace(token, wstr);					// red
					dispToken.replace(token, "<span style=\"color: rgb(255, 0, 0);\">" + token + "</span>");
				}
			}
		}
		
		if (doDate)		
		{	
			// test for date
			QDate d = QDate::fromString(token, "MM/dd/yyyy");
			if (d.isValid())
			{
				QByteArray wstr;
				QByteArray ds = d.toString("MMMM").toLatin1();  // get month as text first
				ds += " ";

//std::cout << "1: " << ds << std::endl;

				QByteArray td = d.toString("dd").toLatin1();
				if (td.size()==1) td.prepend("0");
//std::cout << "  td: " << td << std::endl;
				num2text_date(td, wstr, doDate);
//std::cout << "  wstr: " << wstr << std::endl;
				ds += wstr;
				ds += " ";

//std::cout << "2: " << ds << std::endl;

				QByteArray ty = d.toString("yyyy").toLatin1();
//std::cout << "  ty: " << ty << std::endl;
				num2text(ty, wstr);   // year
//std::cout << "  wstr: " << wstr << std::endl;
				ds += wstr;
				ds += " ";

//std::cout << "3: " << ds << std::endl;

				workToken.replace(token, ds);				// green
				dispToken.replace(token, "<span style=\"color: rgb(0, 170, 0);\">" + token + "</span>");
			}
		}

		if (doRom)		
		{	
			if ( isValidRoman(token) )
			{
				int d = roman2dec(token);
				QByteArray wstr;
				n2t::num2text(QByteArray::number(d), wstr);

				workToken.replace(token, wstr);				// blue
				dispToken.replace(token, "<span style=\"color: rgb(0, 0, 255);\">" + token + "</span>");
			}
		}

		if (doPhone)		
		{	
			if ( isValidPhone(token) )
			{
				QByteArray str = num2text(token);

				workToken.replace(token, str);				// teal
				dispToken.replace(token, "<span style=\"color: rgb(0, 130, 130);\">" + token + "</span>");
			}
		}

		if (doCoord)		
		{	
			int c = 0;
			if ( (c = isValidCoordinate(token)) )
			{
				QByteArray str = coordinate2text(token, c);

				workToken.replace(token, str);				// purple
				dispToken.replace(token, "<span style=\"color: rgb(150, 100, 150);\">" + token + "</span>");
			}
		}

		workStr += workToken + " ";
		dispStr += dispToken + " ";
		
	}  // for loop
}
// ---------------------------------------------------------------


} // namespace


#endif


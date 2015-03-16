#ifndef help_h
#define help_h
static QString help_html ="<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\
 <html><head>\
 \
 \
 \
   <meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"Content-Type\"><title>JCN Tool Plus Guide</title></head><body>\
 <div style=\"text-align: left;\"><span style=\"font-weight: bold;\">JCN\
 Tool&nbsp; Guide</span><br>\
 </div>\
 <br>\
 <a href=\"JCNToolGuide.html#TextEntryGuide\">Text Entry Guide</a><br>\
 <a href=\"#Translate\">Translate Tab</a><br>\
 <a href=\"#Database\">Database Tab</a><br>\
 <a href=\"#FindCompare\">Find/Compare Tab</a><br>\
 <a href=\"#FullMatches\">Full Matches Tab</a><br>\
 <a href=\"#TechDetails\">Technical Details</a><br>\
 <br>\
 When this tool runs for the first time it will create a folder names <span style=\"font-weight: bold; font-style: italic;\">qjcn</span> on<br>\
 your&nbsp;\
 computer's C: drive. <br>\
 <br>\
 It will also place initial <span style=\"font-weight: bold;\"><span style=\"font-style: italic;\">jcn_db.txt</span> </span>and <span style=\"font-weight: bold; font-style: italic;\">jcn_strings.txt</span>&nbsp;&nbsp;\
 in\
 that\
 folder.\
 <br>\
 <span style=\"font-weight: bold; font-style: italic;\"><br>\
 jcn_db.txt</span>&nbsp;\
 is your default database, this is where JCN Tool keeps all entries. <br>\
 <span style=\"font-weight: bold; font-style: italic;\">jcn_strings.txt</span>&nbsp;\
 is\
 where\
 predefined\
 question\
 strings\
 are kept.<br>\
 Along side of <span style=\"font-weight: bold;\">jcn_db.txt</span>\
 another\
 file, named <span style=\"font-weight: bold;\">jcn_db.sql ,</span>&nbsp;\
 will\
 be\
 created.<br>\
 It is used to accelerate searches and can not be edited manually.<br>\
 <br>\
 <span style=\"font-weight: bold;\"></span><a name=\"Translate\"></a><br>\
 <span style=\"font-weight: bold;\">Translate</span><br>\
 Here you will be able to&nbsp;\
 experiment with different strings.<br>\
 Enter any text (<a href=\"#TextEntryGuide\">text entry guidelines</a>)&nbsp;\
 into\
 a\
 text\
 field\
 and\
 press <span style=\"font-weight: bold;\">Translate</span>&nbsp;\
 <br>\
 button on the right<br>\
 Optionally text can be prefixed with one of predefined JCN question\
 string (list on the left). <br>\
 You must select question string that you want. Leave predefined JCN\
 question\
 <br>\
 string selection blank(first entry on the list) if you do not need\
 it. <br>\
 Results of translations can be printed by pressing <span style=\"font-weight: bold;\">Print</span>&nbsp;\
 button and permanently<br>\
 stored in a database for future use by pressing <span style=\"font-weight: bold;\">Add To Database</span>&nbsp; button.<br>\
 Result can also be used to search a database for matches by pressing <span style=\"font-weight: bold;\">Find/Compare</span>&nbsp; button.<br>\
 <br>\
 <a name=\"Database\"></a><br>\
 <span style=\"font-weight: bold;\">Database</span>\
 <br>\
 Here you will be able to&nbsp;\
 browse your JCN database.<br>\
 Use <span style=\"font-weight: bold;\">Search Field</span> to locate any\
 specific text entry<br>\
 Use <span style=\"font-weight: bold;\">Filters</span> and <span style=\"font-weight: bold;\">Apply</span>/<span style=\"font-weight: bold;\">Reset</span> buttons to narrow down\
 or&nbsp;\
 restore&nbsp;\
 your database view.<br>\
 Filters can be applied to text or any number.<br>\
 <br>\
 Use <span style=\"font-weight: bold;\">SQL</span> filter to create\
 custom queries. <br>\
 SQL filter has following form <span style=\"text-decoration: underline; font-weight: bold;\">column=value\
 and </span><span style=\"text-decoration: underline; font-weight: bold;\">column</span><span style=\"text-decoration: underline; font-weight: bold;\">=value or </span><span style=\"text-decoration: underline; font-weight: bold;\">column</span><span style=\"text-decoration: underline; font-weight: bold;\">=value or </span><span style=\"text-decoration: underline; font-weight: bold;\">column</span><span style=\"text-decoration: underline; font-weight: bold;\"> like '%value%'</span>.<br>\
 <br>\
 Use<span style=\"font-weight: bold;\"> = </span>to find exact match<br>\
 Use <span style=\"font-weight: bold;\">like</span> with <span style=\"font-weight: bold;\">%value%</span> or <span style=\"font-weight: bold;\">%value</span> or <span style=\"font-weight: bold;\">value%</span> to find approximate match<br>\
 <br>\
 Valid <span style=\"text-decoration: underline; font-weight: bold;\">column</span>\
 names are (they match columns you see in your database browser)<br>\
 <br>\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; jcntext </span><br style=\"color: rgb(0, 153, 0);\">\
 <br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; f_h</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; f_v</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; f_hv</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; f_c</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; f_p</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; f_pc</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; f_cl</span><br style=\"color: rgb(0, 153, 0);\">\
 <br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; b_h</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; b_v</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; b_hv</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; b_c</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; b_p</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; b_pc</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; b_cl</span><br style=\"color: rgb(0, 153, 0);\">\
 <br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; fb_h</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; fb_v</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; fb_hv</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; fb_c</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; fb_p</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; fb_pc</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; fb_cl</span><br style=\"color: rgb(0, 153, 0);\">\
 <br>\
 <br>\
 This is a very powerful feature. You will be able to look for\
 information stored in you database <br>\
 in a variety of ways. The only limit is your imagination.<br>\
 <br>\
 Examples of SQL filters:<br>\
 <br>\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; f_h=66</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; f_h like '%66%'</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; f_h like '%34%'</span><br>\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; f_h like '%34'</span><br>\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; f_h like '34%'</span><br>\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; f_h=88 and\
 b_h=92</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; (f_h=88 and\
 b_h=92) or (f_h=88+1 and b_h=92-1) or (f_h=88-1 and b_h=92+1)</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; f_h like '%88%'\
 and b_h like '%86%'</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; jcntext like\
 '%reiki%'</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; jcntext like\
 '%reiki%' and f_h=111</span><br style=\"color: rgb(0, 153, 0);\">\
 <span style=\"color: rgb(0, 153, 0);\">&nbsp;&nbsp;&nbsp; jcntext like\
 '%reiki%' and (f_h=111 or f_h=99)</span><br style=\"color: rgb(0, 153, 0);\">\
 <br>\
 Use <span style=\"font-weight: bold;\">Find/Compare</span> button to\
 search for matches.<br>\
 You can also right click to select an entry and use <span style=\"font-weight: bold;\">Find/Compare</span> to search for matches.<br>\
 <br>\
 <span style=\"font-weight: bold;\"><a name=\"FindCompare\"></a><br>\
 Find/Compare</span> <br>\
 Here you will be able find and compare various strings.<br>\
 You can come here from other tabs (Translate, Database) or enter any\
 text you wish into an\
 entry field.<br>\
 Use <span style=\"font-weight: bold;\">Opt</span> button to choose your\
 matching methods and criteria.<br>\
 You can also right click to select an entry and use <span style=\"font-weight: bold;\">Find/Compare</span> to search for matches.<br>\
 <br>\
 <a name=\"FullMatches\"></a><br>\
 <span style=\"font-weight: bold;\">Full Matches</span>\
 <br>\
 Here you will be able see all entries that have full 21 number match<br>\
 Choose Full Matches under Misc menu entry to start it. <br>\
 <span style=\"color: rgb(255, 0, 0);\">Beware, this process will&nbsp;\
 take&nbsp;\
 a&nbsp; while&nbsp;\
 to run</span>.<br>\
 <br>\
 <a name=\"TextEntryGuide\"></a><br>\
 <span style=\"font-weight: bold;\">Text\
 Entry Guide<span style=\"font-weight: bold;\"><br>\
 </span></span>JCN Tool operates on text only. It will translate\
 anything as long as it is one of <br>\
 26 letters of modern Latin alphabet (A-Z). In other words,&nbsp;\
 anything that is<span style=\"font-weight: bold;\"> <br>\
 <span style=\"text-decoration: underline;\">not\
 a letter</span></span><span style=\"text-decoration: underline;\"> </span>will\
 not\
 contribute\
 to\
 a\
 final\
 result.<br>\
 <br>\
 Optionally this tool will translate&nbsp; numbers,&nbsp; dates,&nbsp;\
 Roman\
 numerals,&nbsp; <br>\
 phone numbers and coordinates. <br>\
 <br>\
 In order\
 for it to work this tools will actually convert&nbsp; numbers,&nbsp;\
 dates&nbsp; and&nbsp;\
 <br>\
 numerals&nbsp; to&nbsp; their&nbsp; textual&nbsp;\
 representations.<br>\
 <br>\
 You can choose if you want this feature by selecting either <br>\
 <span style=\"font-weight: bold;\">Translate\
 Numbers</span>, <span style=\"font-weight: bold;\">Translate\
 Dates (Cardinal&nbsp; or&nbsp; Ordinal), Translate Coordinates,<br>\
 Translate Roman Num</span>\
 or <span style=\"font-weight: bold;\">Translate Phone</span>&nbsp; under<span style=\"font-weight: bold;\"> Settings</span> menu. <br>\
 They\
 are optional and are\
 off by default. <br>\
 As these translations happen sections of you original string will be\
 highlighted in <br>\
 following colors :&nbsp; <br>\
 &nbsp;&nbsp;&nbsp; general numbers\
 - <span style=\"color: rgb(255, 0, 0);\">red</span>,&nbsp; <br>\
 &nbsp;&nbsp;&nbsp; dates - <span style=\"color: rgb(51, 204, 0);\">green</span>,&nbsp;\
 <br>\
 &nbsp;&nbsp;&nbsp; romans - <span style=\"color: rgb(51, 51, 255);\">blue</span>,&nbsp;\
 <br>\
 &nbsp;&nbsp;&nbsp; phone - <span style=\"color: rgb(102, 204, 204);\">teal</span>,\
 <br>\
 &nbsp;&nbsp;&nbsp; coordinates - <span style=\"color: rgb(255, 153, 255);\">purple</span><br>\
 <br>\
 <br>\
 Examples of valid numbers and their conversions:<br>\
 <table style=\"width: 732px; height: 269px;\" border=\"1\" cellpadding=\"2\" cellspacing=\"2\">\
   <tbody>\
     <tr>\
       <td>123<br>\
       </td>\
       <td> ONE HUNDRED TWENTY THREE</td>\
     </tr>\
     <tr>\
       <td>1212<br>\
       </td>\
       <td> ONE THOUSAND TWO HUNDRED TWELVE</td>\
     </tr>\
     <tr>\
       <td>1 2 3<br>\
       </td>\
       <td> ONE TWO THREE</td>\
     </tr>\
     <tr>\
       <td>0<br>\
       </td>\
       <td> ZERO</td>\
     </tr>\
     <tr>\
       <td>0 2<br>\
       </td>\
       <td> ZERO TWO</td>\
     </tr>\
     <tr>\
       <td>02<br>\
       </td>\
       <td> TWO</td>\
     </tr>\
     <tr>\
       <td>3.14<br>\
       </td>\
       <td> THREE POINT ONE FOUR</td>\
     </tr>\
     <tr>\
       <td>123.456<br>\
       </td>\
       <td> ONE HUNDRED TWENTY THREE POINT FOUR FIVE SIX</td>\
     </tr>\
   </tbody>\
 </table>\
 <br>\
 Invalid Numbers:<br>\
 <span style=\"color: rgb(255, 0, 0);\">&nbsp;&nbsp;&nbsp; .12<br>\
 &nbsp;&nbsp;&nbsp; 1,234</span><br style=\"color: rgb(255, 0, 0);\">\
 <span style=\"color: rgb(255, 0, 0);\">&nbsp;&nbsp;&nbsp; abc123</span><br>\
 <br>\
 Examples of valid dates and their conversions (cardinal or ordinal):<br>\
 <table style=\"width: 737px; height: 62px;\" border=\"1\" cellpadding=\"2\" cellspacing=\"2\">\
   <tbody>\
     <tr>\
       <td>03/04/2009<br>\
       </td>\
       <td> MARCH FOUR TWO THOUSAND\
 NINE or MARCH FOURTH TWO THOUSAND NINE</td>\
     </tr>\
     <tr>\
       <td>10/05/2010<br>\
       </td>\
       <td> OCTOBER FIVE TWO THOUSAND\
 TEN or OCTOBER FIFTH TWO THOUSAND TEN</td>\
     </tr>\
   </tbody>\
 </table>\
 <br>\
 Invalid dates:<br>\
 &nbsp;&nbsp;&nbsp; <span style=\"color: rgb(255, 0, 0);\">3/4/2009</span><br style=\"color: rgb(255, 0, 0);\">\
 <span style=\"color: rgb(255, 0, 0);\">&nbsp;&nbsp;&nbsp; 03/04/09</span><br>\
 <span style=\"font-weight: bold;\"></span><br>\
 Examples of valid Roman (any word that contains only MDCLXVI&nbsp;\
 letters)&nbsp;\
 and their conversions:<br>\
 <table style=\"width: 629px; height: 152px;\" border=\"1\" cellpadding=\"2\" cellspacing=\"2\">\
   <tbody>\
     <tr>\
       <td>VI<br>\
       </td>\
       <td> SIX</td>\
     </tr>\
     <tr>\
       <td>IX<br>\
       </td>\
       <td> NINE</td>\
     </tr>\
     <tr>\
       <td>XXI<br>\
       </td>\
       <td> TWENTY ONE</td>\
     </tr>\
     <tr>\
       <td>CDXLVIII<br>\
       </td>\
       <td> FOUR HUNDRED FORTY EIGHT</td>\
     </tr>\
     <tr>\
       <td>MMDCCLI<br>\
       </td>\
       <td> TWO THOUSAND SEVEN\
 HUNDRED FIFTY ONE</td>\
     </tr>\
   </tbody>\
 </table>\
 <br>\
 <br>\
 Examples of valid Phone Numbers (or ss#)<br>\
 <table style=\"width: 630px; height: 128px;\" border=\"1\" cellpadding=\"2\" cellspacing=\"2\">\
   <tbody>\
     <tr>\
       <td>123-123-1234</td>\
     </tr>\
     <tr>\
       <td>123-1234</td>\
     </tr>\
     <tr>\
       <td>1 0 0 4 123-123-1234</td>\
     </tr>\
     <tr>\
       <td>123-12-1234</td>\
     </tr>\
   </tbody>\
 </table>\
 <br>\
 Examples of valid Coordinates<br>\
 <table style=\"text-align: left; width: 630px; height: 58px;\" border=\"1\" cellpadding=\"2\" cellspacing=\"2\">\
   <tbody>\
     <tr>\
       <td style=\"vertical-align: top;\">32o02'54.68\"N</td>\
       <td style=\"vertical-align: top;\">Coordinates in Degrees, Minutes,\
 Seconds,<br>\
 Must be entered as shown with no spaces.<br>\
 ##o##'##\"N<br>\
       </td>\
     </tr>\
     <tr>\
       <td style=\"vertical-align: top;\">35.598955,-105.88623</td>\
       <td style=\"vertical-align: top;\">Coordinates in latitude,longitude<br>\
 Two numbers separated by comma with no spaces<br>\
       </td>\
     </tr>\
   </tbody>\
 </table>\
 <br>\
 Invalid coordinates:<br>\
 &nbsp;&nbsp;&nbsp; <span style=\"color: rgb(255, 0, 0);\">32 o 02 '\
 54.68 \" N&nbsp; - no spaces allowed</span><br>\
 <span style=\"color: rgb(255, 0, 0);\">&nbsp;&nbsp;&nbsp; 35.598955,\
 -105.88623 - no spaces allowed</span><br style=\"color: rgb(255, 0, 0);\">\
 <span style=\"color: rgb(255, 0, 0);\">&nbsp;&nbsp;&nbsp; </span><br>\
 <span style=\"font-weight: bold;\"></span><br>\
 <br>\
 <a name=\"TechDetails\"></a><br>\
 <span style=\"font-weight: bold;\">Technical Details</span><br>\
 <br>\
 About JCN Databases<br>\
 Both&nbsp; <span style=\"font-weight: bold;\"><span style=\"font-style: italic;\">jcn_db.txt</span>&nbsp; </span>and&nbsp; <span style=\"font-weight: bold; font-style: italic;\">jcn_strings.txt</span>&nbsp;\
 are\
 plain&nbsp;\
 ASCII<em>(American Standard Code <br>\
 for Information\
 Interchange</em>)&nbsp; text&nbsp; files, editable&nbsp; with&nbsp;\
 any&nbsp; text&nbsp; editor (notepad,\
 wordpad&nbsp;\
 etc). <br>\
 You can also use word processors like Microsoft Word (or others) as\
 long you save final result <br>\
 as a <span style=\"text-decoration: underline;\">Plain Text</span> file.\
 It is done\
 in File/Save As option. If you use notepad or any other editor please <br>\
 make&nbsp; sure&nbsp; to&nbsp; disable&nbsp; 'Word&nbsp; Wrap'&nbsp;\
 option.&nbsp; (your&nbsp; database&nbsp; will&nbsp; get&nbsp;\
 messed&nbsp; up&nbsp; otherwise). <br>\
 Organization of JCN database is simple, one entry per line (JCN Tool\
 uses new line separators to <br>\
 determine entries) Do not run JCN Tool while you edit database files\
 manually. <br>\
 Along side\
 of&nbsp; <span style=\"font-weight: bold;\">jcn_db.txt</span>&nbsp;\
 there&nbsp; will be&nbsp; <span style=\"font-weight: bold;\">jcn_db.sql</span>&nbsp;\
 file.\
 This\
 is\
 a\
 binary\
 version of your <br>\
 JCN database and<span style=\"text-decoration: underline;\"> can not </span>be\
 edited\
 manually.\
 It\
 is\
 used\
 to accelerate searches.<br>\
 When you manually add or remove entries to/from <span style=\"font-weight: bold;\">jcn_db.txt&nbsp;</span> JCN tool will\
 automatically <br>\
 rebuild binary database.<br>\
 <br>\
 JCN Tool will perform reasonably well as long as you have enough memory\
 in your computer. <br>\
 (I would recommend&nbsp; 2GB RAM, or even better 4GB) <br>\
 <br>\
 JCN Tool can run on variety of computers and operating system\
 (Windows, Linux/Unix, MacOS) <br>\
 as long as you have JCN binary for your specific platform. It is\
 written in C++ language and makes\
 <br>\
 extensive use of&nbsp; Qt toolkit and SQLite database.<br>\
 <br>\
 \
 </body></html>";
#endif

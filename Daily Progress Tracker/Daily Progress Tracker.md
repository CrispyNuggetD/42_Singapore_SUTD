# Daily Progress Tracker

## Shortcuts

### 2025

- [Mid-Nov](#mid-nov)
- [Dec](#dec)

### 2026

- [Jan](#jan)
- [Feb](#feb)
- [Mar](#mar)
- [Apr](#apr)

---

# 2025

## Jul (Piscine)
#### 21/07 - Onwards

## Aug (Piscine)
#### 15/08 - Piscine ends 

## Sept, Oct, Nov (Waiting Period):
- Therapy
- Computer textbooks 

## Mid-Nov
### W1
#### 17/11 (Mon 1): 
- Orientation, nearly got TIG’d for coffee :’\)
#### 18/11 (Tues 2):
- Sick
#### 19/11 (Wed 3):
- Sick + Doc’s Appt + Family stuff
#### 20/11 (Thurs 4):
- Sick
#### 20/11 (Fri 5):
- Went school late (Still Sick)
#### 22/11 (WkEnd, Sat 6) WFH (Still Sick):
- Set up working environment (iPad + Keyboard + “Working Copy” IDE/ Application for Git Versioning).
- ChatGPT “refined” Post-piscine custom project instructions for “Computer Science” folder/ project
- Admin (SUTD email, slack, etc…)
- Therapy
#### 23/11 (WkEnd, Sun 7) WFH (Still Sick):
- Continue setting up working environment (ChatGPT custom instructions to fit 42 A.I. pedagogy).
            
### W2


#### 24/11 (Mon 8):
- Join Run Club as Core first time
- Added zsh setup guide for curproj (cd to cur. proj. + Check git status), 
- Also zsh setup guide for syncproj (copy cur. proj to public GitHub - THIS - and git push), 
- Synced folder libft.
#### 25/11 (Tues 9):
- Reported to campus without sleep, even after Track (10*400M) intervals...
- (Previous day is 8 hours of sleep after a night before that of skipping sleep again)
- Studied requirements of Libft (PDF).
- Careful “re-solidifying” of C fundamentals/ Piscine contents.
- Started libft/isalpha
- Learnt itoa, 5 algos (simple, -ve trick, efficient, hardcoding, theoretical/ cute/dumb i.e. recursion)
#### 26/11 (Wed 10):
- (Malloc, char**) “re-solidifying” of C fundamentals/ Piscine contents
- Alex senior taught 7 bitwise operations ```(^``` XOR, ```|```,``` &```, ```~``` inversion, ```!``` NOT, ```<<``` bit shift, ```>>```\)
- Learnt file descriptors (fd), and how UNIX represents things as “files”
- write(fd), where 1 (stdout) and 2 (stderr) both point to terminal, but 1 can be piped to file.
- How ```ft_putchar_fd``` is deceptively simple (need test edge cases but don’t go too extreme than libc)
- Alex corrected ```ft_putchar_fd```‘s char c requires addressing (in hindsight, duh!)
- Alex also taught how bitwise can be used for comparison (isprintable, iseven in isdigit…)
- Alex also explained why HIS IMPLEMENTATION isdigit functions returns in powers of 2 (helps bitwise comparison).
- But apparently ChatGPT says that’s secretly cursed because powers of 2 is internal, guaranteed returns are 0, 1.
- Alex however (anyway) advised for purpose of libft, we “don’t have to go until so complicated”, i.e. return 1,0 good enough.
- Ran 2.95KM to end-of-PIE Plane Spotting area with the gang!
#### 27/11 (Thurs 11):
- Made ```Study Notes.md```, something I should have started long ago w. ChatGPT learning
- Inspiration from alleow's notes.txt
- Learnt a lot about structs and enums today (and using them together for simple reading).
- Learnt (side-track) what "computation" even is (and alternative computation models); how can one not know what they're learning?
- Computation: At its barest bones, computation means performing a rule-governed transformation of information.
-	A system is computing when:
	1. You can describe its state.
	2. You can describe a rule for evolving that state.
	3. The rule produces reproducible outputs.
- Learnt LUTs (look-up tables). And how isdigit() can be mapped to ASCII, cast as unsigned char to index LUT, written as such giving O(1).
- Learnt LUTs' space-time trade off (faster but heavier; more speed, but more memory)
- Learnt (roughly) how bitwise code is supposed to be written properly, not abused (i.e. Libft)
- How to check diff in git repo. (synced?)

```
git fetch
git status
git diff HEAD..origin/main
```

#### 28/11 (Fri 12):
- Went to campus just for 15 mins. 
- Tried fixing study notes broken link. Undone. 
#### 29/11 (WkEnd, Sat 13):
- Out of campus, repair glasses and family birthday celebration. 
#### 30/11 (WkEnd, Sun 14):
- Out of campus (Charity event, Run For Rice 3.0, volunteer.give.asia/activity/run-for-rice-3)

## Dec
### W3
#### 01/12 (Mon 15):
- Urgent wisdom tooth extraction, suffered for 2 days straight afterwards. (Bleeding complications)
#### 02/12 (Tues 16):
- Complete resting due to wisdom tooth.
- Midnight revision on whether we can have a “sentinel struct linkedd list”
#### 03/12 (Wed 17):
- Learnt how to, wrote tutorial, made Makefile and libft.h
- I guess I started on Libft proper. 43 functions to go. 
#### 04/12 (Thurs 18):
- Did makefile corrections; tab means make echos; @ to silent
- Painstakingly installed iSH on iPad with Clang/ norminette support
#### 05/12 (Fri 19):
- Did, failed 24 hours challenge without knowing it’s within a day 0000-2359
- Started ITOA, lots of errors logically and for Norm using canonical method
- ChatGPTherapy
#### 06/12 (WkEnd, Sat 20):
- ITOA troubleshooting canonical method by making helper functions
#### 07/12 (WkEnd, Sun 21):
- MY FIRST LIBFT ITOA IS D.O.N.E. OMG..! ONE STEP FURTHER FROM BLACKHOLE (not proud...) *sobs*
- Finished 4 Libft functions, left 39

### W4
#### 08/12 (Mon 22):
- Finished all ft_is* series + memset + bzero!
#### 09/12 (Tues 23):
- Unwell, MC, hibernated basically
#### 10/12 (Wed 24):
- Post-hibernation sluggishness, lethargic the whole day.
- Finished up till memcopy (to check correctness) (another 2; 13 of 43 done)
#### 11/12 (Thurs 25):
- 11.5 hours working. Genuinely horrifying. I still like coding, so- that's fine I guess.
- Finished part 1 functions.
#### 12/12 (Fri 26):
- Started part 2 functions
#### 13/12 (WkEnd, Sat 27):
- Started ```ft_split```
- Learnt VSC GDB debugging (thanks ```@thtay```!)
- ```ft_split``` debugging pain.
#### 14/12 (WkEnd, Sun 28):
- ```ft_split``` debugging.
- Did putchar series and finished part 2.
- Started linked lists.

### W5
#### 15/12 (Mon 29):
- Finished coding linked lists but last one lstmap having memory leaks.
#### 16/12 (Tues 30):
- Fixed double malloc invalid freeing bug in ft_split, hence finishing libft! :D
- Finished proj. except read me.
#### 17/12 (Wed 31):
- Finished README for libft, proj. complete
- Submitted Proj, not yet eval
- Mock eval by ```@thtay``` (thanks!!)
#### 18/12 (Thurs 32):
- Stressing out over live coding for evaluations; revising for evaluations. 
- Losing sleep over evals is kinda overkill but yea. 
#### 19/12 (Fri 33):
- PASSED Libft evals..!!!!
- 2nd and 3rd was ok. 
- 1st eval chose isalnum for live coding; I can’t believe I actually didn’t manage to do my int Main(void) properly because I was soooo sleep deprived. 
- IMHO, she shouldn’t have passed me but she did anyway. I’ll buck up I guess. 
#### 20/12 (WkEnd, Sat 34):
- I took a break after ```Libft```!!
- Ended up hibernating the day away. 
#### 21/12 (WkEnd, Sun 35):
- Evaluating others:
- A Born2beroot that I could've potentially failed in hindsight due to lack of interest in answering my questions (Borderline, but blackholing. But she could sysadmin from memory so for what it's worth it's good enough for this project).
- Remi’s excellent minishell

### W6
#### 22/12 (Mon 36):
- Came in late: friend meet up last evening.
- ChatGPT about printf learning ways to do it.
- Readme printf update of the ways I used ChatGPT (Proj. Exploration).
#### 23/12 (Tues 37):
- ```@bralee``` peer learning/ whiteboard code jam and lesson on Milestone 1:
	- Makefiles
	- Tips and Tricks
	- Get Next Line
	- Clarifications (Pipex, minishell actual bash piping both is child)
- GNL Tutorial from @bralee (Thank you!!)
- Debate on meaning of “do not change ```logic``` during evaluation re-coding.
	- My Qn about logic change vs data change (printf indexing table) 
- Debate on meaning of “refactoring”, whether any features should change or just code structure.
#### 24/12 (Wed 38):
- More ChatGPT, learn ```typedefs``` and ```void``` function pointers.
- Updated study notes, daily progress tracker.
#### 25/12 (Thurs 39):
- Merry Christmas! I’m bored, so why not study something I love?
- Made ```ft_printf``` makefile, learnt and links to libft in other directory using -I flag.
- Edited libft makefile for clarity (for evaluator) and correctness.
- Makefile works and test of main function of ```ft_printf``` able to call Libft’s ```ft_itoa```.
- Updated main repo Readme.md to account for current 1 month progress and this week’s goals.
- Updated printf Readme for description and explanation of how my program works (the 3 modules).  
#### 24/12 (Wed 38):
- More ChatGPT, learn typedefs and void function pointers.
- Updated study notes, daily progress tracker.
#### 25/12 (Thurs 39):
- Merry Christmas! I'm bored, so why not study something I love?
- Made ft_printf makefile, learnt and links to libft in other directory using -I flag.
- Edited libft makefile for clarity (for evaluator) and correctness.
- Makefile works and test of main function of ```ft_printf``` able to call Libft's ```ft_itoa```.
- Updated main repo Readme.md to account for current 1 month progress and this week's goals.
- Updated printf Readme for description and explanation of how my program works (the 3 modules).
#### 26/12 (Fri 40):
- Started serious progress on ```ft_printf```, learning from ChatGPT.
- Decided to do BOTH bonuses 'cos why not ;\) I get to learn and stand to benefit from it much too.
- Decided on framework with extensibility for modular programming design (parser, dispatcher, handler)
- Uses function pointers
#### 27/12 (WkEnd, Sat 41):
- MC
#### 28/12 (WkEnd, Sun 42):
- Spent 2 hours making not-so-random .zshrc scripts for convenience 
- (and also fun, i.e. 50 hours intra tracker for points)
- Another script to show (all) session details for days that I come in before noon 
- (That's for personal 42 learning journey's accountability/ punctuality)
- Large chunk of ```ft_printf``` parser made
- libc comparison and edge tests to understand requirement for code,
- i.e. no warning flags, cc compilation during evals

### W7
#### 29/12 (Mon 43):
- Spent another 2 hours adding dailylogin function to .zshrc, daily log on/off automations.
- Separated/ refactored .c files to a parsing .c, updated headers.
- Defined bitmask bitshifting values to use with bitmapping using turning on or off or toggling (```|=``` or ```&=~opp``` or ```^=flip```)
- Learnt toggling/ inverting everything except a bitwise operation (```^= ~a```)

#### 30/12 (Tues 44):
- Didn't feel well, slept in. (MC)
- Plus, overseas relatives came over unannounced at night, hosted relatives
#### 31/12 (Wed 45):
- Had to host relatives
- Happy 2026, lol

# 2026

# Jan
#### 01/01 (Thurs 46):
- Started using stdarg.h, using ```va_lists``` as parameters.
- Define function pointer typedef in headers
- Define a struct, ```t_spec```, to hold specs/ flags
- Another struct called tcontext, context holder for passing mutable address (pointers to ```t_specs``` AND ```va_list``` input)
- By EOD, rough script for printing decimals and integers ('d' and 'i' which printf is similar).
- Struct handling in main .c
#### 02/01 (Fri 47):
- ```ft_printf``` compiles finally with types and prototypes and makefiles sorted
- Redo via ChatGPT for VSC tasks.json and launch.json, as not available from Libft (deleted during repo proj. clean-up)
- Made function to print % (for '```%%```')
- Norminetting and helper function .c file (currently only holds error handling ```va_end``` and returns ```-1```)
- Corrected early return inf. loop SIGSEGV bug 
- (cannot return "return" of handler, need check separately for error first, as while loop.)
- (Hence, debugged by making a ptr to point to current ptr for norm-friendly "printer" function with ptr advancement)
- Studied edge cases (like %q) that I have to code later (EDIT 04/01/26: Not needed conversion!)
- Removed warning flag compilation to test cc warning edge cases error handling.
- Pulled all-nighter "for-fun" 'cos why not? Embracing 42 Core while I am privileged to be here.
- By midnight, backlog edit Daily Prog. Tracker for 26/12 onwards, lol. Using git commit details. (Gon' take break now)
#### 03/01 (WkEnd, Sat 48):
- Handler builds a (new, third) ```t_print``` struct with context info for actual printing template
- ```t_print``` passed to ```printer``` function to parse with actual output format printing.
- Learnt various programming knowledge with ChatGPT
- Massive refactoring, addition of info to ```README.md``` of ```ft_printf```
- Got to network, open up, and talk to a lot of my peers!
- Ended 42 marathon after 9am->12mn->4pm (31 hours)
#### 04/01 (WkEnd, Sun 49):
- ```ft_printf``` now prevents partial printing of ```C``` strings with ```write_all_guaranteed```
- ```write_repeat``` repeats one character. Calls ```write_all_guaranteed``` to not duplicate error handling.
- Did an ```itoa_no_sign``` implementation that does not use malloc (uses pointers and changes buffer immediately)
- This ```itoa_no_sign``` also doesn't return sign (duh), as sign is stored in ```struct``` (e.g. prevents ```--42``` being printed)
- Building of ```t_print``` relevant in ```d_i.c``` completed.
- All other building of remainder of ```t_print``` done in ```printer.c```
- ```if``` clamping done to prevent underflows (when core_len > precision and subtract from precision = 0);
- TO-DO: Prototype static functions (that are in reverse order).
- TO-DO: Update ```ft_printf``` ```Readme.md```
- TO-DO: Work on ```send_for_printing``` by understanding the multi-line comment in code about the 3 different output formats (e.g. pad left/ right)

### W8
#### 05/01 (Mon 50):
- Not feeling well/ overworked (MC)
#### 06/01 (Tues 51):
- Refectoring including printer split to ```print_config``` for len, etc. calculations before actual printing.
- Update makefile
#### 07/01 (Wed 52):
- Libfttester for d/i bonus for ```0``` (of d/i) passes
- Basic (most) norminette errors settled
- Some error values and refactoring.
#### 08/01 (Thurs 53):
- Finished checking and printf at 3:45am
#### 09/01 (Fri 54):
- Evaluation and live coding went well. Apparently it's not a new conversion like ```%f``` float. Just custom specifier and inserting predefined string.
- But! (GAHH!!) Moulinette failed, uninitialised ```temp_arg```.
- ```cc``` didn't warn as somehow mapped to ```gcc``` although supposed to be ```clang```.
#### 10/01 (WkEnd, Sat 55);
- SUTD Funny Machines Finals (Attended as public, wrote and shared write-ups)
- Typing practice with mechanical keyboard loaned from ```zchoo``` (thanks!)
#### 11/01 (WkEnd, Sun 56):
- ```ft_printf``` complete, moving on to ```get_next_line```

### W9
#### 12/01 (Mon 57):
- Overnight, ChatGPT till morning
- Derived segmented tables, counting sort for ```push_swap```
- Learnt that properly defining problems scope and universe size is most important for tackling problem statements
#### 13/01 (Tues 58):
- Overnight
- Badminton
- Chat with ```twtan``` and ```thtay```.
- Discussion (```twtan```) of ```push_swap``` techniques (techniques - recursion sort of ```twtan```)
- Discussion (```twtan```) of ```get_next_line``` (edge cases? Null terminator in file? Possible to implement with flags in caller but in scope??)
#### 14/01 (Wed 59):
- Socialising and connecting with peers.
- Mental break/ physical exhaustion (I'm going home for a good rest tonight!!)
#### 15/01 (Thurs 60):
- Evaluated 1 person during points sale (2 points)
- Started coding ```get_next_line``` (42 mins elapsed).
- Stayed overnight on campus.
#### 16/01 (Fri 61):
- 4 more evals during points sale (14 + 2 = 16 total)
- 18 points balance
#### 17/01 (WkEnd, Sat 62):
- Had to go home for DHL delivery
#### 18/01 (WkEnd, Sun 63):
- Unproductive due to worsening mental health (and also OCD)

### W10
#### 19/01 (Mon 64):
- OCD-induced depressive breakdown
- Attempted recovery, minimal, no typing, whiteboard code designing
#### 20/01 (Tues 65):
- Largely more productive. Typing at 20% speed but managed to finish first complete draft of ```get_next_line``` in 9 hours by dinner. 
- Took more than an hour to retroactively update this ```README.md``` at 10% OCD typing speed! :/(
#### 21/01 (Wed 66):
#### 22/01 (Thurs 67):
#### 23/01 (Fri 68):
#### 24/01 (WkEnd, Sat 69):
#### 25/01 (WkEnd, Sun 70):


### W11
#### 26/01 (Mon 71):
#### 27/01 (Tues 72):
#### 28/01 (Wed 73):
#### 29/01 (Thurs 74):
#### 30/01 (Fri 75):
#### 31/01 (WkEnd, Sat 76):

# Feb
#### 01/02 (WkEnd, Sun 77):

### W12
#### 02/02 (Mon 78):
#### 03/02 (Tues 79):
#### 04/02 (Wed 80):
#### 05/02 (Thurs 81):
#### 06/02 (Fri 82):
#### 07/02 (WkEnd, Sat 83):
#### 08/02 (WkEnd, Sun 84):

### W13
#### 09/02 (Mon 85):
#### 10/02 (Tues 86):
#### 11/02 (Wed 87):
#### 12/02 (Thurs 88):
#### 13/02 (Fri 89):
#### 14/02 (WkEnd, Sat 90):
#### 15/02 (WkEnd, Sun 91):

### W14
#### 16/02 (Mon 92):
#### 17/02 (Tues 93):
#### 18/02 (Wed 94):
#### 19/02 (Thurs 95):
#### 20/02 (Fri 96):
#### 21/02 (WkEnd, Sat 97):
#### 22/02 (WkEnd, Sun 98):

### W15
#### 23/02 (Mon 99):
#### 24/02 (Tues 100):
#### 25/02 (Wed 101):
#### 26/02 (Thurs 102):
#### 27/02 (Fri 103):
#### 28/02 (WkEnd, Sat 104):

# Mar
#### 01/03 (WkEnd, Sun 105):

### W16
#### 02/03 (Mon 106):
#### 03/03 (Tues 107):
#### 04/03 (Wed 108):
#### 05/03 (Thurs 109):
#### 06/03 (Fri 110):
#### 07/03 (WkEnd, Sat 111):
#### 08/03 (WkEnd, Sun 112):

### W17
#### 09/03 (Mon 113):
#### 10/03 (Tues 114):
#### 11/03 (Wed 115):
#### 12/03 (Thurs 116):
#### 13/03 (Fri 117):
#### 14/03 (WkEnd, Sat 118):
#### 15/03 (WkEnd, Sun 119):

### W18
#### 16/03 (Mon 120):
#### 17/03 (Tues 121):
#### 18/03 (Wed 122):
#### 19/03 (Thurs 123):
#### 20/03 (Fri 124):
#### 21/03 (WkEnd, Sat 125):
#### 22/03 (WkEnd, Sun 126):

### W19
#### 23/03 (Mon 127):
#### 24/03 (Tues 128):
#### 25/03 (Wed 129):
#### 26/03 (Thurs 130):
#### 27/03 (Fri 131):
#### 28/03 (WkEnd, Sat 132):
#### 29/03 (WkEnd, Sun 133):

### W20
#### 30/03 (Mon 134):
#### 31/03 (Tues 135):

# Apr
#### 01/04 (Wed 136):
#### 02/04 (Thurs 137):
#### 03/04 (Fri 138):
#### 04/04 (WkEnd, Sat 139):
#### 05/04 (WkEnd, Sun 140):


### W21
#### 06/04 (Mon 141):
#### 07/04 (Tues 142):
#### 08/04 (Wed 143):
#### 09/04 (Thurs 144):
#### 10/04 (Fri 145):
#### 11/04 (WkEnd, Sat 146):
#### 12/04 (WkEnd, Sun 147):

### W22
#### 13/04 (Mon 148):
#### 14/04 (Tues 149):
#### 15/04 (Wed 150):
#### 16/04 (Thurs 151):
#### 17/04 (Fri 152):
#### 18/04 (WkEnd, Sat 153):
#### 19/04 (WkEnd, Sun 154):

### W23
#### 20/04 (Mon 155):
#### 21/04 (Tues 156):
#### 22/04 (Wed 157):
#### 23/04 (Thurs 158):
#### 24/04 (Fri 159):
#### 25/04 (WkEnd, Sat 160):
#### 26/04 (WkEnd, Sun 161):

### W24
#### 27/04 (Mon 162):
#### 28/04 (Tues 163):
#### 29/04 (Wed 164):
#### 30/04 (Thurs 165):

# May
#### 01/05 (Fri 166):
#### 02/05 (WkEnd, Sat 167):
#### 03/05 (WkEnd, Sun 168):

### W25
#### 04/05 (Mon 169):
#### 05/05 (Tues 170):
#### 06/05 (Wed 171):
#### 07/05 (Thurs 172):
#### 08/05 (Fri 173):
#### 09/05 (WkEnd, Sat 174):
#### 10/05 (WkEnd, Sun 175):

### W26
#### 11/05 (Mon 176):
#### 12/05 (Tues 177):
#### 13/05 (Wed 178):
#### 14/05 (Thurs 179):
#### 15/05 (Fri 180):
#### 16/05 (WkEnd, Sat 181):
#### 17/05 (WkEnd, Sun 182):

### W27
#### 18/05 (Mon 183):
#### 19/05 (Tues 184):
#### 20/05 (Wed 185):
#### 21/05 (Thurs 186):
#### 22/05 (Fri 187):
#### 23/05 (WkEnd, Sat 188):
#### 24/05 (WkEnd, Sun 189):

### W28
#### 25/05 (Mon 190):
#### 26/05 (Tues 191):
#### 27/05 (Wed 192):
#### 28/05 (Thurs 193):
#### 29/05 (Fri 194):
#### 30/05 (WkEnd, Sat 195):
#### 31/05 (WkEnd, Sun 196):

# Jun
### W29
#### 01/06 (Mon 197):
#### 02/06 (Tues 198):
#### 03/06 (Wed 199):
#### 04/06 (Thurs 200):
#### 05/06 (Fri 201):
#### 06/06 (WkEnd, Sat 202):
#### 07/06 (WkEnd, Sun 203):

### W30
#### 08/06 (Mon 204):
#### 09/06 (Tues 205):
#### 10/06 (Wed 206):
#### 11/06 (Thurs 207):
#### 12/06 (Fri 208):
#### 13/06 (WkEnd, Sat 209):
#### 14/06 (WkEnd, Sun 210):

### W31
#### 15/06 (Mon 211):
#### 16/06 (Tues 212):
#### 17/06 (Wed 213):
#### 18/06 (Thurs 214):
#### 19/06 (Fri 215):
#### 20/06 (WkEnd, Sat 216):
#### 21/06 (WkEnd, Sun 217):

### W32
#### 22/06 (Mon 218):
#### 23/06 (Tues 219):
#### 24/06 (Wed 220):
#### 25/06 (Thurs 221):
#### 26/06 (Fri 222):
#### 27/06 (WkEnd, Sat 223):
#### 28/06 (WkEnd, Sun 224):

### W33
#### 29/06 (Mon 225):
#### 30/06 (Tues 226):

# Jul
#### 01/07 (Wed 227):
#### 02/07 (Thurs 228):
#### 03/07 (Fri 229):
#### 04/07 (WkEnd, Sat 230):
#### 05/07 (WkEnd, Sun 231):

### W34
#### 06/07 (Mon 232):
#### 07/07 (Tues 233):
#### 08/07 (Wed 234):
#### 09/07 (Thurs 235):
#### 10/07 (Fri 236):
#### 11/07 (WkEnd, Sat 237):
#### 12/07 (WkEnd, Sun 238):

### W35
#### 13/07 (Mon 239):
#### 14/07 (Tues 240):
#### 15/07 (Wed 241):
#### 16/07 (Thurs 242):
#### 17/07 (Fri 243):
#### 18/07 (WkEnd, Sat 244):
#### 19/07 (WkEnd, Sun 245):

### W36
#### 20/07 (Mon 246):
#### 21/07 (Tues 247):
#### 22/07 (Wed 248):
#### 23/07 (Thurs 249):
#### 24/07 (Fri 250):
#### 25/07 (WkEnd, Sat 251):
#### 26/07 (WkEnd, Sun 252):

### W37
#### 27/07 (Mon 253):
#### 28/07 (Tues 254):
#### 29/07 (Wed 255):
#### 30/07 (Thurs 256):
#### 31/07 (Fri 257):

# Aug
#### 01/08 (WkEnd, Sat 258):
#### 02/08 (WkEnd, Sun 259):

### W38
#### 03/08 (Mon 260):
#### 04/08 (Tues 261):
#### 05/08 (Wed 262):
#### 06/08 (Thurs 263):
#### 07/08 (Fri 264):
#### 08/08 (WkEnd, Sat 265):
#### 09/08 (WkEnd, Sun 266):

### W39
#### 10/08 (Mon 267):
#### 11/08 (Tues 268):
#### 12/08 (Wed 269):
#### 13/08 (Thurs 270):
#### 14/08 (Fri 271):
#### 15/08 (WkEnd, Sat 272):
#### 16/08 (WkEnd, Sun 273):

### W40
#### 17/08 (Mon 274):
#### 18/08 (Tues 275):
#### 19/08 (Wed 276):
#### 20/08 (Thurs 277):
#### 21/08 (Fri 278):
#### 22/08 (WkEnd, Sat 279):
#### 23/08 (WkEnd, Sun 280):

### W41
#### 24/08 (Mon 281):
#### 25/08 (Tues 282):
#### 26/08 (Wed 283):
#### 27/08 (Thurs 284):
#### 28/08 (Fri 285):
#### 29/08 (WkEnd, Sat 286):
#### 30/08 (WkEnd, Sun 287):

### W42
#### 31/08 (Mon 288):

# Sept

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
- Orientation, nearly got TIG’d for coffee :’)
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
- Alex senior taught 7 bitwise operations (^ XOR, |, &, ~ inversion, ! NOT, << bit shift, >>)
- Learnt file descriptors (fd), and how UNIX represents things as “files”
- write(fd), where 1 (stdout) and 2 (stderr) both point to terminal, but 1 can be piped to file.
- How ft_putchar_fd is deceptively simple (need test edge cases but don’t go too extreme than libc)
- Alex corrected ft_putchar_fd‘s char c requires addressing (in hindsight, duh!)
- Alex also taught how bitwise can be used for comparison (isprintable, iseven in isdigit…)
- Alex also explained why HIS IMPLEMENTATION isdigit functions returns in powers of 2 (helps bitwise comparison).
- But apparently ChatGPT says that’s secretly cursed because powers of 2 is internal, guaranteed returns are 0, 1.
- Alex however (anyway) advised for purpose of libft, we “don’t have to go until so complicated”, i.e. return 1,0 good enough.
- Ran 2.95KM to end-of-PIE Plane Spotting area with the gang!
#### 27/11 (Thurs 11):
- Made Study Notes.md, something I should have started long ago w. ChatGPT learning
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
- Midnight revision on whether we can have a “sentinel struct linked list”
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
- Started ft_split
- Learnt VSC GDB debugging (thanks thtay!)
- ft_split debugging pain.
#### 14/12 (WkEnd, Sun 28):
- ft_split debugging.
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
- Mock eval by thtay (thanks!!)
#### 18/12 (Thurs 32):
- Stressing out over live coding for evaluations; revising for evaluations. 
- Losing sleep over evals is kinda overkill but yea. 
#### 19/12 (Fri 33):
- PASSED Libft evals..!!!!
- 2nd and 3rd was ok. 
- 1st eval chose isalnum for live coding; I can’t believe I actually didn’t manage to do my int Main(void) properly because I was soooo sleep deprived. 
- IMHO, she shouldn’t have passed me but she did anyway. I’ll buck up I guess. 
#### 20/12 (WkEnd, Sat 34):
— Took a break after Libft!!
- Ended up hibernating the day away. 
#### 21/12 (WkEnd, Sun 35):
- Evaluating others:
- A Born2beroot that I should’ve failed in hindsight (borderline, blackholing)
- Remi’s excellent minishell

### W6
#### 22/12 (Mon 36):
- Came in late: friend meet up last evening.
- ChatGPT about printf learning ways to do it.
- Readme printf update of the ways I used ChatGPT (Proj. Exploration).
#### 23/12 (Tues 37):
- @bralee peer learning/ whiteboard code jam and lesson on Milestone 1:
	- Makefiles
	- Tips and Tricks
	- Get Next Line
	- Clarifications (Pipex, minishell actual bash piping both is child)
- GNL Tutorial from @bralee (Thank you!!)
- Debate on meaning of “do not change ```logic``` during evaluation re-coding.
	- My Qn about logic change vs data change (printf indexing table) 
- Debate on meaning of “refactoring”, whether any features should change or just code structure.
#### 24/12 (Wed 38):
- More ChatGPT, learn typedefs and void function pointers.
- Updated study notes, daily progress tracker.
#### 25/12 (Thurs 39):
- Merry Christmas! I’m bored, so why not study something I love?
- Made ft_printf makefile, learnt and links to libft in other directory using -I flag.
- Edited libft makefile for clarity (for evaluator) and correctness.
- Makefile works and test of main function of ft\_printf able to call Libft’s ft\_itoa.
- Updated main repo Readme.md to account for current 1 month progress and this week’s goals.
- Updated printf Readme for description and explanation of how my program works (the 3 modules).  
#### 24/12 (Wed 38):
#### 25/12 (Thurs 39):
#### 26/12 (Fri 40):
#### 27/12 (WkEnd, Sat 41):
#### 28/12 (WkEnd, Sun 42):

### W7
#### 29/12 (Mon 43):
#### 30/12 (Tues 44):
#### 31/12 (Wed 45):

# 2026

# Jan
#### 01/01 (Thurs 46):
#### 02/01 (Fri 47):
#### 03/01 (WkEnd, Sat 48):
#### 04/01 (WkEnd, Sun 49):

### W8
#### 05/01 (Mon 50):
#### 06/01 (Tues 51):
#### 07/01 (Wed 52):
#### 08/01 (Thurs 53):
#### 09/01 (Fri 54):
#### 10/01 (WkEnd, Sat 55):
#### 11/01 (WkEnd, Sun 56):

### W9
#### 12/01 (Mon 57):
#### 13/01 (Tues 58):
#### 14/01 (Wed 59):
#### 15/01 (Thurs 60):
#### 16/01 (Fri 61):
#### 17/01 (WkEnd, Sat 62):
#### 18/01 (WkEnd, Sun 63):

### W10
#### 19/01 (Mon 64):
#### 20/01 (Tues 65):
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


1. make quine to generate c code that loops and checks itself against BFS and exclude patterns (for n=5) that is not covered by heuristics and make new pattern observation
2. find patterns for n=6
3. Start by deciding features, what will OUTPUT look like (edge cases like fully sorted)
4. understand post quickly like spend a glanze and take out points to read or find out more or revisit
5. by today code hardcoded cases 1-5 (n=0 and 1, print NOTHING, all sorted print NOTHING. non-int values, duplicates rmb to code errors)
6. Make BFS (brute force is BFS or other methods like DFS applicable?)
7. Make script that outputs to text file NEATLY all tested situations and a hash to continue where left off, document the organisation in the readme.
8. start with mandatory least first so maybe is it 100 numbers?
9. -
10. from 9, think of 'features" to implement (A* and heuristics, including trimming branches strategies?)
11. "can you trim BFS" or no by definition? Must all paths be explored? can they be built on top of each "number"? i.e. BFS for knowing 10 to 11 numbers can be derived from the steps needed for that 10 instead? Or is it more likely that when the stash size changes from 10 to 11 that the whole technique and "ideal moves" will be different because the 11th number has different "rank" in the answer and that would entirely change the strategy needed to sort the 11 and that HENCE it cann't be derived? Or can it? Analyse
12. From (7), check the heuristics, what % is covered first, and what n is not covered. 
13. Analyse and create heuristics for the remainder n numbers of cases, plausibly with help of chatgpt too
14. Code a general "inefficient" MVP sorting first (Radix?) with the proper org/ data structures, inc. thinking libft printf gnl usage. (do it yourself!! Is radix the best? chunking the best? turk weight technique -- cost and entropy from memory, but only AFTER mvp is done, LIS? what other techniques can i think off other than hardcoding and brute force?) Essentially, this step is MVP, and aim for correctness than no. of steps.
15. know how to check answers and then verify algo with binary checker, ensure correctness
16. Incrementally improve script.
17. Mathematical analysis with ChatGPT, potential to collaborate with others too. Read more blog articles and think of novel ways inc. that (is it hallucated) PID and other techniques.
18. Makefile and "packaging" project, (refer to darren github or ask for help on how to makefile the darned libft lmaooo).

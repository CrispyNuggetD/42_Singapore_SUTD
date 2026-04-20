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
19. Check ChatGPT PS and put into readme such as LaTex for mathematical proofs.
20. Format is: This sentence uses $\` and \`$ delimiters to show math inline: $`\sqrt{3x-1}+(1+x)^2`$

 
i took 1 day to set up.
time log: started PS at 6pm apr 16 which is blackhole pace 24, 26d6h days left:

possible outputs:

perfectly unsorted LIS = 0, entropy = 1
perfectly sorted, LIS = buf_len(), entropy = 0
half sorted, entropy ~0.5 (>0.3 && <0.7)
almost unsorted, entropy ~1 (>=0.7)
almost sorted, entropy ~0 (<=0.3)

So is it right to say entropy is LIS / total cbuf_len()?

- Entropy changes over the sort with every element added or removed, depending on LIS i think? Or maybe not. 
	- It's about how "not the same as the entire stack, i.e. different that element is with the entire stack, wherein a different number will reduce entropy. But if it reduces, will or must it affect LIS? the thing is LIS is integer but entropy is continuous float. Even if LIS doesn't and the loating entropy would have changed.
- Does that mean that using entropy is better than current mnethods of using LIS? why is no one using it? Is it because it is in the new cursus, i.e. the subject.pdf makers "knows" about the entropy properties of the two stacks.
- Is it a computer science CS thing? Where "entropy" of an array is how varying the elements are????
- What other CS knowledge can I apply? (ask chatgpt)

Functions needed:
1. cbuf_len(cbuf *stack, ...?) 
	- Checks and return the length of the stack given, expressed as a circular buffer.
	- By right is stack A or stack B. But is it possible to self-split each stack into "mini-stacks"? 
		- Is that literally "chunking"? is there a difference? Is "chunking" still expressed as a cbuf and is the length needed to be known or is it pre-defined (chicken and egg?)
2. post_optimisation(cbuf *stack, ...?)
	- Performs basic as well as advanced functions to optimise sequence of operations found thus far (as inputed into function). 
	- It can be ran after every "chunk" or as a final optimisation, depending on necessity.
	- Basic checks:
		1.1. check_remove_nonsensical_moves()
	- Advanced modifications:
		2.1. remove_redundant_moves()
			2.1.1. remove_duplications() i.e. pb,pa
			2.1.2. 

best_least_moves()
	- n_is_1(); (???????)
	- n_is_2();
	- n_is_3();
	- n_is_4(); (???????)
	- n_is_5();
	- n_is_6(); (???????)

bruteforce()
	- bfs();
	- a_star(); (???????)
		- Uses heuristics to trim unnecessary branches; optimised bfs(); (???????)

export_to_file()
	- Export current run info to file
		- Sequence used (n=total length, the_sequence)
		- Edge cases found: (Total number, numbered cases)
		- Hash iteration value reached: (hash used, n=iteration)


get_statistics(stats *new_empty_stats_object)
	-  returns "stats" struct type

structs:
1. stats:
	- float entropy;
	- int lis; //longest_increasing_sequence
	- (?type) variance;
	- (?type) std_devation;
	- float mean;
	- int mode;
	- int highest_freq;
	- int median;
	- int max;
	- int min;
	- int highest_difference; // max-min
	- int least_difference;



-------------------------------------------------
Links to put in readme for education resources used:
1. Worst Case Analysis: Selection Sort vs Insertion Sort - https://www.youtube.com/shorts/wJayBp4r4_4
	- A direct performance comparison of Selection and Insertion Sort on a descending array (worst-case scenario). This visualization highlights the mechanical differences: Selection Sort’s constant scanning for the minimum versus Insertion Sort’s heavy element shifting within the sorted prefix. An essential look at algorithm behavior under stress.
2. 90 Sorts on Large Inputs - Scatter Plot - https://www.youtube.com/watch?v=0HFPiwmnNgo
3. The Bubble Sort Curve
	- A derivation of the curve that is approximated by a common visualization of the bubble sort diagram.
	- Read the full proof on my site: https://linesthatconnect.github.io/blog/a-rigorous-derivation-of-the-bubble-sort-curve/
	- After uploading, I learned that this result appeared in the paper "Nonparametric testing via partial sorting" by Krzysztof Bisewski, H. M. Jansen, and Yoni Nazarathy. Read it here: https://arxiv.org/abs/2210.14546
	- The viral sorting algorithm video which first sparked my interest:    • 15 Sorting Algorithms in 6 Minutes  
		4. https://www.youtube.com/watch?v=kPRA0W1kECg
	- The animations in this video were created using Manim: https://www.manim.community/
5. Stackmaxxing for a recursion world record - https://www.youtube.com/watch?v=WQKSyPYF0-Y
	- I believe I’ve set a world record for the deepest recursive function call: over 17 trillion nested invocations.  It’s literally the most profound thing I've ever done with a computer.
	- 05:06 The stack grows up, not down!
	- 10:52 FUSE
	- 15:41 NULL pointer? No problem!
	- 20:54 Kernel patching
	- 21:22 Far call to 32-bit mode
	- 26:28 Ideas for how to recurse 1000x more than I did.
	- Links:
		- Recursion: A Novel https://amzn.to/4r9ajld
		- The Linux Programming Interface https://amzn.to/4adwXmO

=====================

So the valid elements live in the circular interval:

[read_idx, write_idx)

=================

```c
int	cbuf_rev_rotate(cbuf *stack)
{
	if (cbuf_len(stack) < 2)
		return (1);
	...
}
```

Why do we check cbuf_len(stack) < 2?

Because:

empty stack → nothing to rotate
one element → rotating changes nothing

So for correctness and safety, rotate only matters when there are at least 2 elements.
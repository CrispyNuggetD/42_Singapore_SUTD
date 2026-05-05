# Book: INTRODUCTION TO ALGORITHMS
- THIRD EDITION (2009)

## Authors:
- THOMAS H. CORMEN
- CHARLES E. LEISERSON
- RONALD L. RIVEST
- CLIFFORD STEIN

## Contents
1. [Book Contents + Preface Preview (Only Sample till /Preface/To the professional)](#1-book-contents—preface-preview-only-sample-till-prefacetothe-professional)
2. [Glossary](#2-glossary)
2. [ascsacascscscacsacascasacascascasca 5 Compiler optimisations for Clang (My not-so-secret high perf. obsession!)](#2-five-compiler-optimisations-for-clang-my-not-so-secret-high-perf-obsession)

##### [Back to top of contents list](#contents)

## Note to self:
- As much I love going into rabbit holes, sometimes it's way beyond what I can handle (e.g. at Week 2 Core)
- Hence, checklists denotes to learn OR learnt thereafter (when I have time)
- Formatting: 
- [ ] To learn (the below) when I have time
- [x] Learnt!
- Also, for things I'm particularly interested or that they're relevant to me, they get: **(IMPORTANT FOR ME)
  
-—-

### 1. Book Contents + Preface Preview (Only Sample till /Preface/To the professional)

—--

> ### NOTE: (***) means graduate level content

### Book Contents

- Preface xiii

### I Foundations

- Introduction 3

#### 1 The Role of Algorithms in Computing 5

- 1.1 Algorithms 5
- 1.2 Algorithms as a technology 11

#### 2 Getting Started 16

- 2.1 Insertion sort 16
- 2.2 Analyzing algorithms 23
- 2.3 Designing algorithms 29

#### 3 Growth of Functions 43

- 3.1 Asymptotic notation 43
- 3.2 Standard notations and common functions 53

#### 4 Divide-and-Conquer 65

- 4.1 The maximum-subarray problem 68
- 4.2 Strassen’s algorithm for matrix multiplication 75
- 4.3 The substitution method for solving recurrences 83
- 4.4 The recursion-tree method for solving recurrences 88
- 4.5 The master method for solving recurrences 93
- (***) 4.6 Proof of the master theorem 97

#### 5 Probabilistic Analysis and Randomized Algorithms 114

- 5.1 The hiring problem 114
- 5.2 Indicator random variables 118
- 5.3 Randomized algorithms 122
- (***) 5.4 Probabilistic analysis and further uses of indicator random variables 130

### II Sorting and Order Statistics

- Introduction 147

#### 6 Heapsort 151
- 6.1 Heaps 151
- 6.2 Maintaining the heap property 154
- 6.3 Building a heap 156
- 6.4 The heapsort algorithm 159
- 6.5 Priority queues 162

#### 7 Quicksort 170
- 7.1 Description of quicksort 170
- 7.2 Performance of quicksort 174
- 7.3 A randomized version of quicksort 179
- 7.4 Analysis of quicksort 180

#### 8 Sorting in Linear Time 191
- 8.1 Lower bounds for sorting 191
- 8.2 Counting sort 194
- 8.3 Radix sort 197
- 8.4 Bucket sort 200

#### 9 Medians and Order Statistics 213
- 9.1 Minimum and maximum 214
- 9.2 Selection in expected linear time 215
- 9.3 Selection in worst-case linear time 220
          
### III Data Structures

- Introduction 229

#### 10 Elementary Data Structures 232
- 10.1 Stacks and queues 232
- 10.2 Linked lists 236
- 10.3 Implementing pointers and objects 241
- 10.4 Representing rooted trees 246

#### 11 Hash Tables 253
- 11.1 Direct-address tables 254
- 11.2 Hash tables 256
- 11.3 Hash functions 262
- 11.4 Open addressing 269
- (***) 11.5 Perfect hashing 277

#### 12 Binary Search Trees 286
- 12.1 What is a binary search tree? 286
- 12.2 Querying a binary search tree 289
- 12.3 Insertion and deletion 294
- (***) 12.4 Randomly built binary search trees 299

#### 13 Red-Black Trees 308
- 13.1 Properties of red-black trees 308
- 13.2 Rotations 312
- 13.3 Insertion 315
- 13.4 Deletion 323

#### 14 Augmenting Data Structures 339
- 14.1 Dynamic order statistics 339
- 14.2 How to augment a data structure 345
- 14.3 Interval trees 348

### IV Advanced Design and Analysis Techniques

- Introduction 357

#### 15 Dynamic Programming 359
- 15.1 Rod cutting 360
- 15.2 Matrix-chain multiplication 370
- 15.3 Elements of dynamic programming 378
- 15.4 Longest common subsequence 390
- 15.5 Optimal binary search trees 397

#### 16 Greedy Algorithms 414
- 16.1 An activity-selection problem 415
- 16.2 Elements of the greedy strategy 423
- 16.3 Huffman codes 428
- (***) 16.4 Matroids and greedy methods 437
- (***) 16.5 A task-scheduling problem as a matroid 443

#### 17 Amortized Analysis 451
- 17.1 Aggregate analysis 452
- 17.2 The accounting method 456
- 17.3 The potential method 459

### V Advanced Data Structures

- Introduction 481

#### 18 B-Trees 484
- 18.1 Definition of B-trees 488
- 18.2 Basic operations on B-trees 491
- 18.3 Deleting a key from a B-tree 499

#### 19 Fibonacci Heaps 505
- 19.1 Structure of Fibonacci heaps 507
- 19.2 Mergeable-heap operations 510
- 19.3 Decreasing a key and deleting a node 518
- 19.4 Bounding the maximum degree 523

#### 20 van Emde Boas Trees 531
- 20.1 Preliminary approaches 532
- 20.2 A recursive structure 536
- 20.3 The van Emde Boas tree 545

#### 21 Data Structures for Disjoint Sets 561
- 21.1 Disjoint-set operations 561
- 21.2 Linked-list representation of disjoint sets 564
- 21.3 Disjoint-set forests 568
— (***) 21.4 Analysis of union by rank with path compression 573

### VI Graph Algorithms

- Introduction 587

#### 22 Elementary Graph Algorithms 589
- 22.1 Representations of graphs 589
- 22.2 Breadth-first search 594
- 22.3 Depth-first search 603
- 22.4 Topological sort 612
- 22.5 Strongly connected components 615

#### 23 Minimum Spanning Trees 624
- 23.1 Growing a minimum spanning tree 625

#### 24 Single-Source Shortest Paths 643
- 24.1 The Bellman-Ford algorithm 651
- 24.2 Shortest paths in DAGs 655
- 24.3 Dijkstra’s algorithm 658
- 24.4 Difference constraints and shortest paths 664
- 24.5 Proofs of shortest-paths properties 671

#### 25 All-Pairs Shortest Paths 684
- 25.1 Shortest paths and matrix multiplication 686
- 25.2 The Floyd-Warshall algorithm 693
- 25.3 Johnson’s algorithm for sparse graphs 700

#### 26 Maximum Flow 708
- 26.1 Flow networks 709
- 26.2 The Ford-Fulkerson method 714
- 26.3 Maximum bipartite matching 732
- (***) 26.4 Push-relabel algorithms 736
- (***) 26.5 The relabel-to-front algorithm 748

### VII Selected Topics

- Introduction 769

#### 27 Multithreaded Algorithms 772
- 27.1 Basics of dynamic multithreading 774
- 27.2 Multithreaded matrix multiplication 792
- 27.3 Multithreaded merge sort 797

#### 28 Matrix Operations 813
- 28.1 Solving linear systems 813
- 28.2 Inverting matrices 827
- 28.3 Least-squares approximation 832

#### 29 Linear Programming 843
- 29.1 Standard and slack forms 850
- 29.2 Formulating LP problems 859
- 29.3 The simplex algorithm 864
- 29.4 Duality 879
- 29.5 Initial basic feasible solution 886

#### 30 Polynomials and the FFT 898
- 30.1 Representing polynomials 900
- 30.2 The DFT and FFT 906
- 30.3 Efficient FFT implementations 915

#### 31 Number-Theoretic Algorithms 926
- 31.1 Elementary notions 927
- 31.2 Greatest common divisor 933
- 31.3 Modular arithmetic 939
- 31.4 Modular linear equations 946
- 31.5 Chinese remainder theorem 950
- 31.6 Powers of an element 954
- 31.7 RSA cryptosystem 958
- (***) 31.8 Primality testing 965
- (***) 31.9 Integer factorization 975

#### 32 String Matching 985
- 32.1 Naive algorithm 988
- 32.2 Rabin-Karp 990
- 32.3 Finite automata 995
- (***) 32.4 Knuth-Morris-Pratt 1002

#### 33 Computational Geometry 1014
- 33.1 Line-segment properties 1015
- 33.2 Segment intersection 1021
- 33.3 Convex hull 1029
- 33.4 Closest pair of points 1039

#### 34 NP-Completeness 1048
- 34.1 Polynomial time 1053
- 34.2 Verification 1061
- 34.3 Reducibility 1067
- 34.4 NP-completeness proofs 1078
- 34.5 NP-complete problems 1086

#### 35 Approximation Algorithms 1106
- 35.1 Vertex cover 1108
- 35.2 Traveling salesman problem 1111
- 35.3 Set covering 1117
- 35.4 Randomization and LP 1123
- 35.5 Subset sum 1128

### VIII Appendix: Mathematical Background

- Introduction 1143

#### A Summations 1145
- A.1 Formulas and properties 1145
- A.2 Bounding summations 1149

#### B Sets, Etc. 1158
- B.1 Sets 1158
- B.2 Relations 1163
- B.3 Functions 1166
- B.4 Graphs 1168
- B.5 Trees 1173

#### C Counting and Probability 1183
- C.1 Counting 1183
- C.2 Probability 1189
- C.3 Discrete random variables 1196
- C.4 Geometric and binomial distributions 1201
- (***) C.5 Tails of the binomial distribution 1208

#### D Matrices 1217
- D.1 Matrix operations 1217
- D.2 Matrix properties 1222

#### Bibliography 1231

#### Index 1251

---

### Preface

> Before there were computers, there were algorithms. But now that there are computers, there are even more algorithms, and algorithms lie at the heart of computing. This book provides a comprehensive introduction to the modern study of computer algorithms. It presents many algorithms and covers them in considerable depth, yet makes their design and analysis accessible to all levels of readers. We have tried to keep explanations elementary without sacrificing depth of coverage or mathematical rigor. Each chapter presents an algorithm, a design technique, an application area, or a related topic. Algorithms are described in English and in a pseudocode designed to be readable by anyone who has done a little programming. The book contains 244 figures—many with multiple parts—illustrating how the algorithms work. Since we emphasize efficiency as a design criterion, we include careful analyses of the running times of all our algorithms. The text is intended primarily for use in undergraduate or graduate courses in algorithms or data structures. Because it discusses engineering issues in algorithm design, as well as mathematical aspects, it is equally well suited for self-study by technical professionals. In this, the third edition, we have once again updated the entire book. The changes cover a broad spectrum, including new chapters, revised pseudocode, and a more active writing style.

#### To the teacher

> We have designed this book to be both versatile and complete. You should find it useful for a variety of courses, from an undergraduate course in data structures up through a graduate course in algorithms. Because we have provided considerably more material than can fit in a typical one-term course, you can consider this book to be a “buffet” or “smorgasbord” from which you can pick and choose the material that best supports the course you wish to teach.

> You should find it easy to organize your course around just the chapters you need. We have made chapters relatively self-contained, so that you need not worry about an unexpected and unnecessary dependence of one chapter on another. Each chapter presents the easier material first and the more difficult material later, with section boundaries marking natural stopping points. In an undergraduate course, you might use only the earlier sections from a chapter; in a graduate course, you might cover the entire chapter. We have included 957 exercises and 158 problems. Each section ends with exercises, and each chapter ends with problems. The exercises are generally short questions that test basic mastery of the material. Some are simple self-check thought exercises, whereas others are more substantial and are suitable as assigned homework. The problems are more elaborate case studies that often introduce new material; they often consist of several questions that lead the student through the steps required to arrive at a solution. Departing from our practice in previous editions of this book, we have made publicly available solutions to some, but by no means all, of the problems and exercises. Our Web site, http://mitpress.mit.edu/algorithms/, links to these solutions. You will want to check this site to make sure that it does not contain the solution to an exercise or problem that you plan to assign. We expect the set of solutions that we post to grow slowly over time, so you will need to check it each time you teach the course. We have starred (?) the sections and exercises that are more suitable for graduate students than for undergraduates. A starred section is not necessarily more difficult than an unstarred one, but it may require an understanding of more advanced mathematics. Likewise, starred exercises may require an advanced background or more than average creativity.

#### To the student

> We hope that this textbook provides you with an enjoyable introduction to the field of algorithms. We have attempted to make every algorithm accessible and interesting. To help you when you encounter unfamiliar or difficult algorithms, we describe each one in a step-by-step manner. We also provide careful explanations of the mathematics needed to understand the analysis of the algorithms. If you already have some familiarity with a topic, you will find the chapters organized so that you can skim introductory sections and proceed quickly to the more advanced material. This is a large book, and your class will probably cover only a portion of its material. We have tried, however, to make this a book that will be useful to you now as a course textbook and also later in your career as a mathematical desk reference or an engineering handbook.

##### What are the prerequisites for reading this book?

> - You should have some programming experience. In particular, you should understand recursive procedures and simple data structures such as arrays and linked lists.

> - You should have some facility with mathematical proofs, and especially proofs by mathematical induction. A few portions of the book rely on some knowledge of elementary calculus. Beyond that, Parts I and VIII of this book teach you all the mathematical techniques you will need.

> We have heard, loud and clear, the call to supply solutions to problems and exercises. Our Web site, http://mitpress.mit.edu/algorithms/, links to solutions for a few of the problems and exercises. Feel free to check your solutions against ours. We ask, however, that you do not send your solutions to us.

#### To the professional

> The wide range of topics in this book makes it an excellent handbook on algorithms. Because each chapter is relatively self-contained, you can focus in on the topics that most interest you. Most of the algorithms we discuss have great practical utility. We therefore address implementation concerns and other engineering issues. We often provide practical alternatives to the few algorithms that are primarily of theoretical interest. If you wish to implement any of the algorithms, you should find the translation of our pseudocode into your favorite programming language to be a fairly straightforward task. We have designed the pseudocode to present each algorithm clearly and succinctly. Consequently, we do not address error-handling and other software-engineering issues that require specific assumptions about your programming environment. We attempt to present each algorithm simply and directly without allowing the idiosyncrasies of a particular programming language to obscure its essence. We understand that if you are using this book outside of a course, then you might be unable to check your solutions to problems and exercises against solutions provided by an instructor. Our Web site, http://mitpress.mit.edu/algorithms/, links to solutions for some of the problems and exercises so that you can check your work. Please do not send your solutions to us.

… (continued in actual book)

##### [Back to contents list](#contents)

---

### 2. Glossary

---

#### Preface

1. Recursive Procedures 
2. 
3. 



<TEMP DUMP TO DO:>


 binomial heaps and sorting
networks. One key idea in the sorting networks chapter, the 0-1 principle, ap-
pears in this edition within Problem 8-7 as the 0-1 sorting lemma for compare-
exchange algorithms. The treatment of Fibonacci heaps no longer relies on
binomial heaps as a precursor
We revised our treatment of dynamic programming and greedy algorithms. Dynamic programming now leads off with a more interesting problem, rod cutting, than the assembly-line scheduling problem from the second edition. Furthermore, we emphasize memoization a bit more than we did in the second edition, and we introduce the notion of the subproblem graph as a way to understand the running time of a dynamic-programming algorithm. In our opening example of greedy algorithms, the activity-selection problem, we get to the greedy algorithm more directly than we did in the second edition.  The way we delete a node from binary search trees (which includes red-black trees) now guarantees that the node requested for deletion is the node that is actually deleted. In the first two editions, in certain cases, some other node would be deleted, with its contents moving into the node passed to the deletion procedure. With our new way to delete nodes, if other components of a program maintain pointers to nodes in the tree, they will not mistakenly end up with stale pointers to nodes that have been deleted.  The material on flow networks now bases flows entirely on edges. This approach is more intuitive than the net flow used in the first two editions.  With the material on matrix basics and Strassen’s algorithm moved to other chapters, the chapter on matrix operations is smaller than in the second edition.  We have modified our treatment of the Knuth-Morris-Pratt string-matching algorithm.  We corrected several errors. Most of these errors were posted on our Web site of second-edition errata, but a few were not.  Based on many requests, we changed the syntax (as it were) of our pseudocode. We now use “D” to indicate assignment and “= =” to test for equality, just as C, C++, Java, and Python do. Likewise, we have eliminated the keywords do and then and adopted “//” as our comment-to-end-of-line symbol. We also now use dot-notation to indicate object attributes. Our pseudocode remains procedural, rather than object-oriented. In other words, rather than running methods on objects, we simply call procedures, passing objects as parameters.  We added 100 new exercises and 28 new problems. We also updated many bibliography entries and added several new ones.  Finally, we went through the entire book and rewrote sentences, paragraphs, and sections to make the writing clearer and more active.





<TEMP>













#### Website

- [http://mitpress.mit.edu/algorithms](http://mitpress.mit.edu/algorithms/)

##### [Back to contents list](#contents)

---

### 0. Lorem Ipsum

---

#### Lorem Ipsum: 

##### [Back to contents list](#contents)

---
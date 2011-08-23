This is a simple repository set up to put [all of the test cases](http://www.guiffy.com/SureMergeWP.html#tck1) from [Bill Ritcher's paper](http://www.guiffy.com/SureMergeWP.html) for [Guiffy](http://www.guiffy.com/) describing safe 3-way merge algorithms in one place, with all of the test cases sorted into directories. Using it to test a merge tool is easy:

1. configure the merge tool you want to test in Git.
2. clone this repository.
3. <code>git merge origin/other</code>
4. <code>git mergetool</code>

All copyrights on the source files themselves are clearly marked, and in most cases belong to Bill Ritcher or Guiffy.

I'd love pointers to more examples of pathological merging, the crazier the better.

**NOTE:** The existence of this repository should in no way be construed as an endorsement of Guiffy, although, unsurprisingly, it does the best job of the currently available merge tools in handling the various challenges the test cases throw at it.
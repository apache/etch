
This is Bob Jenkins' hash table code from burtleburtle.net/bob/hash/hashtab.html.
The author, Bob Jenkins, has placed this code in the public domain, 
He states as such in comments in the code itself. Regarding inclusion of the
code in an Apache project, he states:
   I do prefer that you keep a pointer back to me as the source, so I can 
   answer questions if any come up.  I would object if someone claimed 
   they'd written it themselves. But other than that, there's no restrictions, 
   you can remove or rewrite any of the comments or code, 
   including the disclaimers.

   James DeCocq (jadecocq) wrote:
   > Hi Bob,
   > Last year I used your excellent hash table C code in a project which 
   > is now to become an Apache open source project. What is the usual  
   > attribution and licensing disclaimer procedures when folks use your  
   > code in open source projects? 

CUSTOMIZED FOR ETCH
The original code has been customized for use by etch. Changes are as follows:
- renamed files belonging to this subproject to begin with "jenk".
- changed name of unique.c to jenktest.c; changed this to read from file not stdin
- added includes to .c files such that they compile individually

BEGIN JENKINS COMMENTS ON THE CODE
hashtab.h and hashtab.c form the hash table module. 
The files before those are the support files that I always use. 
The file after it (unique.c) is an example of how to use the hash table. 
(The program UNIQUE takes a file in STDIN and dumps the unique lines 
(duplicates removed) to STDOUT. 
It also shuffles the unique lines pseudorandomly. 
The sample input provided doesn't have any duplicate lines, 
so the output should be the same size as the input, but the lines will be shuffled.) 

The hash table has a fixed hash function, and its size is a power of two. 
It doubles in size when it needs to, and when it doubles, it doubles all at once. 
It never shrinks. Input keys and data are pointed to, not copied. 
Keys are unique. 
Collisions are handled by chaining. 

Functions are: 
hcreate  - create a hash table 
hdestroy - destroy a hash table 
hcount   - how many items are in the hash table? 
hkey     - the key at the current position 
hkeyl    - the length of the key at the current position 
hstuff   - the other data at the current position 
hfind    - position the hash table at some key 
hadd     - add a new <key,stuff> pair to the table 
hdel     - delete the item at the current position 
hfirst   - position at the first item in the table 
hnext    - move to the next item in the table 
hstat    - print statistics about this table 

The most unusual thing about this module is that it maintains a current position. 
This means you can't have a dangling pointer into it. 
If you position on something, and then delete it, the position moves to another item. 
On the other hand, it also means it's hard to do nested loops over all the items in the table, 
since there can be only one position at a time. 
END JENKINS COMMENTS ON THE CODE


JENKINS EMAIL RE 64-BIT COMPATIBILITY
I don't have any.  However, Thomas Wang came up with this one:

public long hash64shift(long key)
{
  key = (~key) + (key << 21); // key = (key << 21) - key - 1;
  key = key ^ (key >>> 24);
  key = (key + (key << 3)) + (key << 8); // key * 265
  key = key ^ (key >>> 14);
  key = (key + (key << 2)) + (key << 4); // key * 21
  key = key ^ (key >>> 28);
  key = key + (key << 31);
  return key;
}

I haven't tested it, but the functions of his that I have tested aren't bad, and the operations look about like what I'd expect is needed.  My preliminary stabs at a 64-bit functions said 8 or 9 shifts are needed, he's got 10, but many are done in parallel, so it does look like a promising function.

James DeCocq (jadecocq) wrote:
> Hi Bob,
>
> Have you tried the code in 64 bits? I don't currently have the ability 
> to do so, but a glance at some of the code, specifically the renew() 
> macro, seems as if it might assume that a pointer is the same size as 
> an int. If it is the case that it makes assumptions as to 32 bits, are 
> there specific tweaks you may have made for 64 bits that you can share?
END JENKINS EMAIL RE 64-BIT COMPATIBILITY



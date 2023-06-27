# BispooKeyGen2 Solution
Reversing CTF Solution

# Write-Up
## Objective
1. Find the Anti-Debug techniques.
2. Bypass the Anti-Debug techniques.
3. Find the password for an example username “Benjamin”.
4. Code C program that checks if a pair of username and password fit.
5. My own bonus: Code C program that finds the password for a given username.

## Topics Covered
<ul>
  <li>x32/x64 dbg (A.k.a x96dbg)</li>
  <li>C coding</li>
</ul>

## The idea
1. Performing a dynamic analysis of the program.
2. Patching the program in a way that all Anti-Debug techniques will be bypassed.
3. Code the keygen mechanism in C

## Procedure
![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/default%20bispoo.png)

Let’s try entering “Benjamin” as the username and “1234” as the password.

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/user_Benjamin.png)

Ok… now let’s dive into debugging it to find the password.

By throwing the program to x32dbg and running it there, the program happens not to run.</br>
Smells like Anti-Debug techniques.</br>
Let’s take a look in the Call Stack

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/call%20stack.png)

*Bispookeygen2.00402017* is from where we exited the program.</br>
Let’s grab a look of what’s going on there.

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/asm1.png)

Having *ExitProcess* twice is quite strange.</br>
Let’s put a breakpoint on *0x00402000* and dynamically analyze from there.</br>
I noticed something odd with this line.</br>
Here is how the code looks before running line:

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/asm2.png)

And here is how it looks like after it:

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/asm3.png)

Notice how *DialogBoxParamA* changed to *ExitProcess*.</br>
Let’s see what this strange function does (*0x00402439*).

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/graph1.png)

Bingo. Here is the Anti-Debug.</br>
### The Anti-Debug techniques that are present here are:
1. *IsDebbugerPresent* (i.e. IsBeingDebugged in PEB)</br>
2. Code integrity checks.

But wait, what is the function *0x00402498* up to?

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/graph2.png)

Here is the 3rd Anti-Debug technique:</br>
Checks for breakpoints in places that they are not needed to be present.

### So, to conclude the Anti-Debug techniques:
Anti-Debug Function: *0x00402439*</br>
Techniques:</br>
1. *IsDebbugerPresent* (i.e. IsBeingDebugged in PEB)
2. Code integrity checks.
3. Checks for breakpoints in places that they are not needed to be present.

In order to bypass these Anti-Debug techniques, let’s simply replace every call for this function with nops.

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/bispoo%20again.png)

This is after nopping the call for this function out of the first line.
Now let’s find all the other calls for this function and nop them.
Let’s proceed to finding the password.

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/asm4.png)

According to <a href="https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-dialogboxparama">MSDN</a>, the operation function of the dialog box is *0x00402029*.

After some trial and error, the action area of the *“Try”* button in the function is here

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/graph3.png)

Let μ be the length of username.</br>
We can see that 4 ≤ μ ≤ 15

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/graph4.png)
![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/graph5.png)

Additionally, the first character in the username can’t be a space.

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/graph6.png)

### How the keygen work?
The mechanism is divided into 4 steps.

#### Step 1

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/graph7.png)

This part encodes the username into a sequence of successive bytes and calculates a number that stored in edx.</br>

The equivalent of this part in C will look as follows:

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/c1.png)

#### Step 2

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/graph8.png)

This part multiplies the number that was calculated and stored in edx with the first 4 bytes (int) of the sequential bytes from step 1 and saves this big number in the memory because it is larger than the size of a register.</br>

The equivalent of this part in C will look as follows:

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/c2.png)

#### Step 3

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/graph9.png)

Let’s see what is the function *0x004023b9*

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/graph10.png)

Well, this part encodes each byte of the calculated big number from step 2 by xor-ing the byte *0xbe*.

The equivalent of this part in C will look as follows:

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/c3.png)

Function call:

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/c4.png)

#### Step 4

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/graph11.png)

Let’s see what is in the function *0x004023da*

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/graph12.png)

This part takes the bytes from step 3 and changes every byte that is bigger than *0x8f* and not equal to *0x93* to *0x1*,</br>
and every byte that is equal to *0x93* to *0x8d*.</br>
Finally, it adds to the end *“-LEET”*.

The equivalent of this part in C will look as follows:

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/c5.png)

Function call:

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/c6.png)

At the end, the program checks if the user inputted password is equal to the generated password based on the username.

**I attached 2 C files.**
1. Check.c - checks if a username and password fit.
2. pwn.c - find a correct password based on a given username.

In cmd, let’s run pwn.exe (that is compiled from pwn.c).</br>
Enter *“Benjamin”* and the password is:</br>
**31147352329-LEET**

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/password.png)

##### Let’s try it in the program
Username: **Benjamin**</br>
Password: **31147352329-LEET**

![](https://github.com/ben18mk/BispooKeyGen2_Solution/blob/main/Doc%20Snaps/pwn.png)

Pwned!

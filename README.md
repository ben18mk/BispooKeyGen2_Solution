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

The Karp-Rabin fingerprinting scheme [1] uses polynomials of integers while Rabin's [2] uses polynomials over a finite field. Such class of fingerprinting algorithms has a most significant advantage: rolling efficiency, i.e., computation of a new fingerprint can reuse an overlapped existing fingerprint. Therefore they are suitable for a sliding window manner, such as used in chunking files for deduplication.

Our implementation tailors Karp-Rabin/Rabin's fingerprinting method for sliding windows in three ways:

1. Sliding by one byte. On one hand, either binary or text input strings can be regarded as aligned in bytes without any other assumption on their data structure, so one-byte step is generally applicable. One the other hand, some implementation computing the fingerprint of a whole object instead of sliding fixed-width windows [3] consumes longer input than one byte each step in order to raise efficiency. However, too long step is not suitable for a sliding window, since it would break the shift/insertion-resistance [4], another attractive property provided by the Rabin's class.

2. Based on arithmetic operation of integers. Generally, fingerprints of sliding windows are not big and can be efficiently handled within one-word arithmetic operations. Although the module operation (%) is not as efficient as xor (^) or shift, a table lookup as used in [3] seems no much better (of course, this method is useful when the P(t) is long and out of one-word arithmetic scope, but for short P(t), such extra complexity may introduce unnecessary overhead).

3. Using predefined irreducible polynomials/primes. Random choice of irreducible polynomials/primes withstands malicious construction of strings. However, such a requirement is rarely needed in the practice of Rabin fingerprinting, e.g., string matching or file chunking. So we omit, for the moment, the function that generates randomized irreducible polynomials/primes. 

Reference
[1] Karp, R. M., and Rabin, M. O. 1987. Efficient randomized pattern-matching algorithms. IBM J. Res. Devel. 31, 2, 249-260.
[2] Rabin, M. O. 1981. Fingerprinting by random functions. Report TR-15-81, Center for Research in Computing Technology, Harvard University, Cambridge, MA.
[3] Broder, A. Z. 1993. Some applications of Rabin's fingerprinting method. Sequences II: Methods in Communications, Security, and Computer Science. Springer-Verlag.
[4] Wikipedia. Rabin fingerprint. http://en.wikipedia.org/wiki/Rabin_fingerprint.

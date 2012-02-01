Broader description can be found in ../README or ../../README.

Implemented Algorithms:

Each byte of the input is regarded as a symbol, i.e., there are 2^8 different symbols that constitute input strings.

For an input string X_r of length p: X_r = (x_r, x_(r+1), ... x_(r+p-1)).
The fingerprint of X_r is:
  F(X_r) = [x_r * b^(p-1) + ... + x_(r+p-1)] % q 
where b = 2^8, and q is a predefined prime. All fingerprints shall evenly distribute among [0, q).

We recommend and assume that b < q, and b * q is within the integer scope (either 32-bit or 64-bit). Then for X'_r of length p + 1:
  F(X'_r) = [X_r * b + x_(r+p)] % q
          = [F(X_r) * b + x_(r+p)] % q 
where F(X_r) has been computed.

We denote the above computation as extend(byte). Any string can use this function to iteratively generate its fingerprint (from F(null) = 0).

Furthermore, for fixed length p, we have:
  F(X_(r+1)) = [X_r * b + x_(r+p) - x_r * b^p] % q
             = [F(X_r) * b + x_(r+p) - x_r * R] % q 
where R = (b^p) % q is precomputed.

Note that, in our implementation, extend(byte) and slide(byte) should not be mixed. Any window should be used for either extending to produce a fingerprint of a long string OR sliding to produce multiple fingerprints of sub-strings.


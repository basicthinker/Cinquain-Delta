/**
 * Copyright (C) 2012 Jinglei Ren <jinglei.ren@stanzax.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package org.stanzax.cinquain;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.math.BigInteger;
import java.util.Scanner;

/**
 * RabinFingerprintChecker checks Rabin fingerprints in an input file
 * by comparing them with those computed using BigInteger in a straight forward way.
 * NOTE: the slide-in symbol should be LSB.
 * SimpleWindow uses little-endian integer operations if it resides on x86 architecture,
 * so in that case its results should NOT be checked by this class.
 * <p>
 * The input file should contain a two-line header followed by data lines
 * (all using hexadecimal values):
 * <p>
 * [SymbolRadix] the first line should start with such a hexadecimal integer<br>
 * [Prime] the second line should start with such a hexadecimal integer<br>
 * [NumberOfSymbols] { [Symbol...] } [FingerprintToCheck]<br>
 * ...<br>
 * <p>
 * The output lines have three columns:
 * original fingerprint, correct fingerprint, whether they are equal.
 * 
 * @author Jinglei Ren <jinglei.ren@gmail.com>
 *
 */
public class RabinFingerprintChecker {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if (args.length != 1) {
			System.out.println("Usage: java -jar RabinFingerprintChecker.jar [FileName]");
			return;
		}
		
		try {
			FileReader reader = new FileReader(args[0]);
			Scanner scanner = new Scanner(reader);
			
			// Parse the first two lines to retrieve symbol radix and prime
			long symbolRadix = 0;
			BigInteger prime = new BigInteger("0");
			for (int lineNum = 1; lineNum <= 2; ++lineNum) {
				if (!scanner.hasNext()) {
					System.err.println("Unexpected end of file.");
					return;
				}
				switch (lineNum) {
				case 1:
					symbolRadix = scanner.nextLong(16);
					scanner.nextLine();
					break;
				case 2:
					prime = scanner.nextBigInteger(16);
					scanner.nextLine();
					break;
				default:
					return;
				}
			}
			
			int totalNum = 0;
			int wrongNum = 0;
			BigInteger weight = BigInteger.valueOf(symbolRadix);
			
			while (scanner.hasNext()) {
				int length = scanner.nextInt(16);
				
				scanner.next(); // skip "{"
				BigInteger stringValue = new BigInteger("0");
				for (int i = 0; i < length; ++i) {
					stringValue = stringValue.multiply(weight).add(scanner.nextBigInteger(16));
				}
				scanner.next(); // skip "}"
				
				String correctValue = stringValue.mod(prime).toString(16);
				String toCheck = scanner.next();
				
				++totalNum;
				if (!toCheck.equals(correctValue)) {
					++wrongNum;
				}
				System.out.printf("%s %s %b\n", toCheck, correctValue, toCheck.equals(correctValue));
			}
			
			System.out.println("\nResults:");
			System.out.printf("Checked %d fingerprints.\n", totalNum);
			System.out.printf("There are %d wrong ones.\n", wrongNum);
			
			scanner.close();
			reader.close();
			
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}

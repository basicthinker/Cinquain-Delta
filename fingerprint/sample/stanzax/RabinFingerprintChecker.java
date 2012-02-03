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
package stanzax;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.math.BigInteger;
import java.util.Scanner;

/**
 * @author Jinglei Ren <jinglei.ren@gmail.com>
 *
 */
public class RabinFingerprintChecker {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if (args.length != 1) {
			System.out.println("Usage: ./RabinFingerprintChecker.jar [FileName]");
			return;
		}
		
		try {
			FileReader reader = new FileReader(args[0]);
			Scanner scanner = new Scanner(reader);
			
			// Parse the first two lines to retrieve bit-length of symbol and prime
			long numBits = 0;
			BigInteger prime = new BigInteger("0");
			for (int lineNum = 1; lineNum <= 2; ++lineNum) {
				if (!scanner.hasNext()) {
					System.err.println("Unexpected end of file.");
					return;
				}
				switch (lineNum) {
				case 1:
					numBits = scanner.nextLong(16);
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
			
			BigInteger weight = BigInteger.valueOf(1L << numBits);
			while (scanner.hasNext()) {
				int length = scanner.nextInt(16);
				
				scanner.next("{");
				BigInteger stringValue = new BigInteger("0");
				for (int i = 0; i < length; ++i) {
					stringValue = stringValue.multiply(weight).add(scanner.nextBigInteger(16));
				}
				scanner.next("}");
				
				String stdValue = stringValue.mod(prime).toString();
				String toCheck = scanner.next();
				
				System.out.printf("%s %s %b\n", toCheck, stdValue, toCheck.equals(stdValue));
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}

}

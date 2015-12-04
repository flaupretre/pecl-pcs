
# The PCS test extension

Checking every [PCS](http://pcs.tekwire.net) feature cannot be done using the PHP usual test framework only. An additional extension is required. For various reasons, this extension cannot be included in the PCS source tree. So, it is published here as a separate extension.

## Usage

First, install and activate [the PCS extension](http://pecl.php.net/package/pcs).

Then, compile this extension and run tests :

	phpize
	./configure
	make test

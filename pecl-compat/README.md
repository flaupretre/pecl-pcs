The PECL compatibility library is a set of C include files whose objective is to minimize the pain of making a PHP extension compatible with PHP 5 and 7.

Base directory : [https://github.com/flaupretre/pecl-pcs](https://github.com/flaupretre/pecl-pcs)

# Main features

- A set of miscellaneous macros

- A zend_string library for PHP 5

- An adaptation of most new zend_hash features for PHP 5

# Examples

For examples of using this library, look at the [PCS extension](https://github.com/flaupretre/pecl-pcs). There, you will see usage examples of almost every available features.

# Usage

Using the library is simple. Download the latest release from the [github repository](https://github.com/flaupretre/pecl-compat/releases) and insert it in your code tree. Then, include the 'compat.h' file in every '.c' source file.

# History

I first tried to port some extensions to PHP 7 with a set of '#ifdef' directives. Unfortunately, it quickly became clear that it was not possible without making the code totally unreadable. Of course, for pure 'bridge' extensions, it is possible. But, as soon as you make an extensive use of hash tables and strings, it becomes impossible to maintain.

Then, I explored other solutions, like separate branches or duplicate code trees. But, in my opinion, none of these are really satisfying in terms of maintainability : separate branches bring a huge problem of versioning, and, for obvious reasons, separate code trees are a maintenance headache in the long term. Of course, all of this is negligible if maintenance of PHP 5 modules stops in a couple of months, which someone seem to believe. But I think that's irrealistic and guess we have to maintain PHP 5 versions of our modules during several years.

So, I reverted to the solution of keeping a single source tree and create a compatibility layer. In order to benefit of the PHP 7 performance increase, this library mostly contains a set of PHP 7 features backported to PHP 5. So, when using the library, PHP 7 code most often directly calls the underlying PHP functions. That's important because, using this library, you will both change your code to use new PHP 7 features (like zend_strings) and make it compatible with both versions. As a side effect, this makes easier to discard PHP 5 compatibility in the future.

# Supported PHP versions

Every PHP version starting from 5.3.0.


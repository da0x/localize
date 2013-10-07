localize
========

Localize is a command line tool for parsing objective-c files looking for non localized strings. The tool will then add NSLocalizedString() wrapper around the found strings that have not been localized yet.

This tool takes the content of an objective-c file and replaces @"" with NSLocalizedString(@"",nil)

NSLocalizedString() is used by genstrings utility which generates the .strings file(s) for you.

genstrings : https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man1/genstrings.1.html 

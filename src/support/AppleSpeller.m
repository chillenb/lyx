/**
 * \file AppleSpeller.m
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Stephan Witt
 *
 * Full author contact details are available in file CREDITS.
 */

#import <Cocoa/Cocoa.h>

#import <AvailabilityMacros.h>

#include <wchar.h>

#include "support/AppleSpeller.h"

typedef struct AppleSpellerRec {
	NSSpellChecker * checker;
	NSInteger doctag;
	NSArray * suggestions;
	NSArray * misspelled;
} AppleSpellerRec ;


AppleSpeller newAppleSpeller(void)
{
	AppleSpeller speller = calloc(1, sizeof(AppleSpellerRec));
	speller->checker = [NSSpellChecker sharedSpellChecker];
	speller->doctag = [NSSpellChecker uniqueSpellDocumentTag];
	speller->suggestions = nil;
	speller->misspelled = nil;
	return speller;
}


void freeAppleSpeller(AppleSpeller speller)
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	[speller->checker closeSpellDocumentWithTag:speller->doctag];

	[speller->suggestions release];
	[speller->misspelled release];

	[pool release];

	free(speller);
}


static NSString * toString(const char * lang)
{
	return [[NSString alloc] initWithBytes:lang length:strlen(lang) encoding:NSUTF8StringEncoding];
}


NSString * wcharToString(const wchar_t* text, NSUInteger length)
{
	BOOL lendian = NSHostByteOrder() == NS_LittleEndian;
	NSUInteger bytes = length*sizeof(wchar_t);

	return [[NSString alloc] initWithBytes:text length:bytes encoding:(lendian ? NSUTF32LittleEndianStringEncoding : NSUTF32BigEndianStringEncoding)];
}


static NSString * toLanguage(AppleSpeller speller, const char * lang)
{
	NSString * result = nil;
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	NSString * lang_ = toString(lang);
	if ([NSSpellChecker instancesRespondToSelector:@selector(availableLanguages)]) {
		NSArray * languages = [speller->checker availableLanguages];

		for (NSString *element in languages) {
			if (0 == [element caseInsensitiveCompare:lang_]) {
				result = element;
				break;
			} else if ([lang_ hasPrefix:element]) {
				result = element;
			}
		}
	}
	[lang_ release];
	[pool release];
	return result;
}


BOOL surrorate(unichar curr, unichar next) {
	return 0xD800 <= curr && curr <= 0xDBFF &&
		0xDC00 <= next && next <= 0xDFFF;
}


NSArray * AppleSpeller_adjustPositions(NSArray * misspelled,
	NSString * text, NSUInteger length)
{
	NSUInteger r = 0;
	NSUInteger i = 0;
	NSUInteger rcount = [misspelled count];
	NSRange range = [[misspelled objectAtIndex:r] rangeValue];
	NSUInteger rstart = range.location;
	NSUInteger rend   = range.location+range.length;
	NSUInteger asurrogates = 0;
	NSUInteger rsurrogates = 0;
	NSMutableArray * result = [NSMutableArray arrayWithCapacity:rcount+1];
	unichar curr = [text characterAtIndex:i];

	while (i < length-1 && r < rcount) {
		if (i == rstart) {
			range.location -= asurrogates;
			rsurrogates = 0;
		} else if (i == rend) {
			range.length -= rsurrogates;
			[result addObject:[NSValue valueWithRange:range]];
			if (++r < rcount) {
				range = [[misspelled objectAtIndex:r] rangeValue];
				rstart = range.location;
				rend   = range.location+range.length;
			}
		}
		unichar prev = curr;
		curr = [text characterAtIndex:++i];
		if (surrorate(prev, curr) && i < length-1) {
			curr = [text characterAtIndex:++i];
			asurrogates++;
			rsurrogates++;
		}
	}
	return result;
}


SpellCheckResult AppleSpeller_check(AppleSpeller speller,
	const wchar_t * word, const char * lang)
{
	if (!speller->checker || !lang || !word)
		return SPELL_CHECK_FAILED;

	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	NSUInteger wlength = wcslen(word);
	NSString * word_ = wcharToString(word, wlength);
	NSString * lang_ = toString(lang);
	SpellCheckResult result = SPELL_CHECK_FAILED;
	NSUInteger start = 0;
	NSUInteger ulength = [word_ length];

	[speller->misspelled release];
	speller->misspelled = nil;
	BOOL surrogates = ulength > wlength;

	while (result == SPELL_CHECK_FAILED && start < ulength) {
		NSRange match = [speller->checker
			checkSpellingOfString:word_
			startingAt:start
			language:lang_
			wrap:(BOOL)NO
			inSpellDocumentWithTag:speller->doctag
			wordCount:NULL];

		result = match.length == 0 ? SPELL_CHECK_OK : SPELL_CHECK_FAILED;
		if (result == SPELL_CHECK_OK) {
			if ([NSSpellChecker instancesRespondToSelector:@selector(hasLearnedWord:)]) {
				if ([speller->checker hasLearnedWord:word_])
					result = SPELL_CHECK_LEARNED;
			}
		} else {
			NSUInteger capacity = [speller->misspelled count] + 1;
			NSMutableArray * misspelled = [NSMutableArray arrayWithCapacity:capacity];
			[misspelled addObjectsFromArray:speller->misspelled];
			[misspelled addObject:[NSValue valueWithRange:match]];
			[speller->misspelled release];
			speller->misspelled = [[NSArray arrayWithArray:misspelled] retain];
			start = match.location + match.length + 1;
		}
	}
	if ([speller->misspelled count] > 0 && surrogates) {
		NSArray * misspelled = AppleSpeller_adjustPositions(speller->misspelled, word_, ulength);
		[speller->misspelled release];
		speller->misspelled = [[NSArray arrayWithArray:misspelled] retain];
	}

	[word_ release];
	[lang_ release];
	[pool release];

	return [speller->misspelled count] ? SPELL_CHECK_FAILED : result;
}


void AppleSpeller_ignore(AppleSpeller speller, const wchar_t * word)
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	NSString * word_ = wcharToString(word, wcslen(word));

	[speller->checker ignoreWord:word_ inSpellDocumentWithTag:(speller->doctag)];

	[word_ release];
	[pool release];
}


size_t AppleSpeller_makeSuggestion(AppleSpeller speller, const wchar_t * word, const char * lang)
{
	if (!speller->checker || !word || !lang)
		return 0;

	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	NSString * word_ = wcharToString(word, wcslen(word));
	NSString * lang_ = toString(lang);
	NSArray * result ;

	NSInteger slen = [word_ length];
	NSRange range = { 0, slen };

	result = [speller->checker guessesForWordRange:range
		inString:word_
		language:lang_
		inSpellDocumentWithTag:speller->doctag];

	[word_ release];
	[lang_ release];

	[speller->suggestions release];
	speller->suggestions = [[NSArray arrayWithArray:result] retain];

	[pool release];
	return [speller->suggestions count];
}


const char * AppleSpeller_getSuggestion(AppleSpeller speller, size_t pos)
{
	const char * result = 0;
	if (pos < [speller->suggestions count]) {
		result = [[speller->suggestions objectAtIndex:pos] UTF8String] ;
	}
	return result;
}


void AppleSpeller_learn(AppleSpeller speller, const wchar_t * word)
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	NSString * word_ = wcharToString(word, wcslen(word));

	if ([NSSpellChecker instancesRespondToSelector:@selector(learnWord:)])
		[speller->checker learnWord:word_];

	[word_ release];
	[pool release];
}


void AppleSpeller_unlearn(AppleSpeller speller, const wchar_t * word)
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	NSString * word_ = wcharToString(word, wcslen(word));

	if ([NSSpellChecker instancesRespondToSelector:@selector(unlearnWord:)])
		[speller->checker unlearnWord:word_];

	[word_ release];
	[pool release];
}


int AppleSpeller_numMisspelledWords(AppleSpeller speller)
{
	return [speller->misspelled count];
}


void AppleSpeller_misspelledWord(AppleSpeller speller, int index, int * start, int * length)
{
	NSRange range = [[speller->misspelled objectAtIndex:(NSUInteger)index] rangeValue];
	*start = range.location;
	*length = range.length;
}


int AppleSpeller_hasLanguage(AppleSpeller speller, const char * lang)
{
	return toLanguage(speller, lang) != nil;
}

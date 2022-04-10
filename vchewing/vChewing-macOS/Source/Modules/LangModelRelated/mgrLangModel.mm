// Copyright (c) 2011 and onwards The OpenVanilla Project (MIT License).
// All possible vChewing-specific modifications are of:
// (c) 2021 and onwards The vChewing Project (MIT-NTL License).
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

1. The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

2. No trademark license is granted to use the trade names, trademarks, service
marks, or product names of Contributor, except as required to fulfill notice
requirements above.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#import "mgrLangModel.h"
#import "LMConsolidator.h"
#import "mgrLangModel_Privates.h"
#import "vChewing-Swift.h"

static const int kUserOverrideModelCapacity = 500;
static const double kObservedOverrideHalflife = 5400.0;

static vChewing::LMInstantiator gLangModelCHT;
static vChewing::LMInstantiator gLangModelCHS;
static vChewing::UserOverrideModel gUserOverrideModelCHT(kUserOverrideModelCapacity, kObservedOverrideHalflife);
static vChewing::UserOverrideModel gUserOverrideModelCHS(kUserOverrideModelCapacity, kObservedOverrideHalflife);

static NSString *const kUserDataTemplateName = @"template-data";
static NSString *const kUserAssDataTemplateName = @"template-data";
static NSString *const kExcludedPhrasesvChewingTemplateName = @"template-exclude-phrases";
static NSString *const kPhraseReplacementTemplateName = @"template-phrases-replacement";
static NSString *const kUserSymbolDataTemplateName = @"template-user-symbol-data";
static NSString *const kTemplateExtension = @".txt";

@implementation mgrLangModel

static void LTLoadLanguageModelFile(NSString *filenameWithoutExtension, vChewing::LMInstantiator &lm)
{
    Class cls = NSClassFromString(@"ctlInputMethod");
    NSString *dataPath = [[NSBundle bundleForClass:cls] pathForResource:filenameWithoutExtension ofType:@"txt"];
    lm.loadLanguageModel([dataPath UTF8String]);
}

+ (NSString *)specifyBundleDataPath:(NSString *)filenameWithoutExtension;
{
    Class cls = NSClassFromString(@"ctlInputMethod");
    return [[NSBundle bundleForClass:cls] pathForResource:filenameWithoutExtension ofType:@"txt"];
}

+ (void)loadDataModels
{
    if (!gLangModelCHT.isDataModelLoaded())
    {
        LTLoadLanguageModelFile(@"data-cht", gLangModelCHT);
    }
    if (!gLangModelCHT.isMiscDataLoaded())
    {
        gLangModelCHT.loadMiscData([[self specifyBundleDataPath:@"data-zhuyinwen"] UTF8String]);
    }
    if (!gLangModelCHT.isSymbolDataLoaded())
    {
        gLangModelCHT.loadSymbolData([[self specifyBundleDataPath:@"data-symbols"] UTF8String]);
    }
    if (!gLangModelCHT.isCNSDataLoaded())
    {
        gLangModelCHT.loadCNSData([[self specifyBundleDataPath:@"char-kanji-cns"] UTF8String]);
    }
    // -----------------
    if (!gLangModelCHS.isDataModelLoaded())
    {
        LTLoadLanguageModelFile(@"data-chs", gLangModelCHS);
    }
    if (!gLangModelCHS.isMiscDataLoaded())
    {
        gLangModelCHS.loadMiscData([[self specifyBundleDataPath:@"data-zhuyinwen"] UTF8String]);
    }
    if (!gLangModelCHS.isSymbolDataLoaded())
    {
        gLangModelCHS.loadSymbolData([[self specifyBundleDataPath:@"data-symbols"] UTF8String]);
    }
    if (!gLangModelCHS.isCNSDataLoaded())
    {
        gLangModelCHS.loadCNSData([[self specifyBundleDataPath:@"char-kanji-cns"] UTF8String]);
    }
}

+ (void)loadDataModel:(InputMode)mode
{
    if ([mode isEqualToString:imeModeCHT])
    {
        if (!gLangModelCHT.isDataModelLoaded())
        {
            LTLoadLanguageModelFile(@"data-cht", gLangModelCHT);
        }
        if (!gLangModelCHT.isMiscDataLoaded())
        {
            gLangModelCHT.loadMiscData([[self specifyBundleDataPath:@"data-zhuyinwen"] UTF8String]);
        }
        if (!gLangModelCHT.isSymbolDataLoaded())
        {
            gLangModelCHT.loadSymbolData([[self specifyBundleDataPath:@"data-symbols"] UTF8String]);
        }
        if (!gLangModelCHT.isCNSDataLoaded())
        {
            gLangModelCHT.loadCNSData([[self specifyBundleDataPath:@"char-kanji-cns"] UTF8String]);
        }
    }

    if ([mode isEqualToString:imeModeCHS])
    {
        if (!gLangModelCHS.isDataModelLoaded())
        {
            LTLoadLanguageModelFile(@"data-chs", gLangModelCHS);
        }
        if (!gLangModelCHS.isMiscDataLoaded())
        {
            gLangModelCHS.loadMiscData([[self specifyBundleDataPath:@"data-zhuyinwen"] UTF8String]);
        }
        if (!gLangModelCHS.isSymbolDataLoaded())
        {
            gLangModelCHS.loadSymbolData([[self specifyBundleDataPath:@"data-symbols"] UTF8String]);
        }
        if (!gLangModelCHS.isCNSDataLoaded())
        {
            gLangModelCHS.loadCNSData([[self specifyBundleDataPath:@"char-kanji-cns"] UTF8String]);
        }
    }
}

+ (void)loadUserPhrases
{
    gLangModelCHT.loadUserPhrases([[self userPhrasesDataPath:imeModeCHT] UTF8String],
                                  [[self excludedPhrasesDataPath:imeModeCHT] UTF8String]);
    gLangModelCHS.loadUserPhrases([[self userPhrasesDataPath:imeModeCHS] UTF8String],
                                  [[self excludedPhrasesDataPath:imeModeCHS] UTF8String]);
    gLangModelCHT.loadUserSymbolData([[self userSymbolDataPath:imeModeCHT] UTF8String]);
    gLangModelCHS.loadUserSymbolData([[self userSymbolDataPath:imeModeCHS] UTF8String]);
}

+ (void)loadUserAssociatedPhrases
{
    gLangModelCHT.loadUserAssociatedPhrases([[self userAssociatedPhrasesDataPath:imeModeCHT] UTF8String]);
    gLangModelCHS.loadUserAssociatedPhrases([[self userAssociatedPhrasesDataPath:imeModeCHS] UTF8String]);
}

+ (void)loadUserPhraseReplacement
{
    gLangModelCHT.loadPhraseReplacementMap([[self phraseReplacementDataPath:imeModeCHT] UTF8String]);
    gLangModelCHS.loadPhraseReplacementMap([[self phraseReplacementDataPath:imeModeCHS] UTF8String]);
}

+ (BOOL)checkIfUserDataFolderExists
{
    NSString *folderPath = [self dataFolderPath:false];
    BOOL isFolder = NO;
    BOOL folderExist = [[NSFileManager defaultManager] fileExistsAtPath:folderPath isDirectory:&isFolder];
    if (folderExist && !isFolder)
    {
        NSError *error = nil;
        [[NSFileManager defaultManager] removeItemAtPath:folderPath error:&error];
        if (error)
        {
            NSLog(@"Failed to remove folder %@", error);
            return NO;
        }
        folderExist = NO;
    }
    if (!folderExist)
    {
        NSError *error = nil;
        [[NSFileManager defaultManager] createDirectoryAtPath:folderPath
                                  withIntermediateDirectories:YES
                                                   attributes:nil
                                                        error:&error];
        if (error)
        {
            NSLog(@"Failed to create folder %@", error);
            return NO;
        }
    }
    return YES;
}

+ (BOOL)checkIfSpecifiedUserDataFolderValid:(NSString *)folderPath
{
    BOOL isFolder = NO;
    BOOL folderExist = [[NSFileManager defaultManager] fileExistsAtPath:folderPath isDirectory:&isFolder];
    if ((folderExist && !isFolder) || (!folderExist))
    {
        return NO;
    }
    return YES;
}

+ (BOOL)ensureFileExists:(NSString *)filePath
    populateWithTemplate:(NSString *)templateBasename
               extension:(NSString *)ext
{
    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath])
    {

        NSURL *templateURL = [[NSBundle mainBundle] URLForResource:templateBasename withExtension:ext];
        NSData *templateData;
        if (templateURL)
        {
            templateData = [NSData dataWithContentsOfURL:templateURL];
        }
        else
        {
            templateData = [@"" dataUsingEncoding:NSUTF8StringEncoding];
        }

        BOOL result = [templateData writeToFile:filePath atomically:YES];
        if (!result)
        {
            NSLog(@"Failed to write file");
            return NO;
        }
    }
    return YES;
}

+ (BOOL)checkIfUserLanguageModelFilesExist
{
    if (![self checkIfUserDataFolderExists])
        return NO;
    if (![self ensureFileExists:[self userPhrasesDataPath:imeModeCHS]
            populateWithTemplate:kUserDataTemplateName
                       extension:kTemplateExtension])
        return NO;
    if (![self ensureFileExists:[self userPhrasesDataPath:imeModeCHT]
            populateWithTemplate:kUserDataTemplateName
                       extension:kTemplateExtension])
        return NO;
    if (![self ensureFileExists:[self userAssociatedPhrasesDataPath:imeModeCHS]
            populateWithTemplate:kUserAssDataTemplateName
                       extension:kTemplateExtension])
        return NO;
    if (![self ensureFileExists:[self userAssociatedPhrasesDataPath:imeModeCHT]
            populateWithTemplate:kUserAssDataTemplateName
                       extension:kTemplateExtension])
        return NO;
    if (![self ensureFileExists:[self excludedPhrasesDataPath:imeModeCHS]
            populateWithTemplate:kExcludedPhrasesvChewingTemplateName
                       extension:kTemplateExtension])
        return NO;
    if (![self ensureFileExists:[self excludedPhrasesDataPath:imeModeCHT]
            populateWithTemplate:kExcludedPhrasesvChewingTemplateName
                       extension:kTemplateExtension])
        return NO;
    if (![self ensureFileExists:[self phraseReplacementDataPath:imeModeCHS]
            populateWithTemplate:kPhraseReplacementTemplateName
                       extension:kTemplateExtension])
        return NO;
    if (![self ensureFileExists:[self phraseReplacementDataPath:imeModeCHT]
            populateWithTemplate:kPhraseReplacementTemplateName
                       extension:kTemplateExtension])
        return NO;
    if (![self ensureFileExists:[self userSymbolDataPath:imeModeCHT]
            populateWithTemplate:kUserSymbolDataTemplateName
                       extension:kTemplateExtension])
        return NO;
    if (![self ensureFileExists:[self userSymbolDataPath:imeModeCHS]
            populateWithTemplate:kUserSymbolDataTemplateName
                       extension:kTemplateExtension])
        return NO;
    return YES;
}

+ (BOOL)checkIfUserPhraseExist:(NSString *)userPhrase
                     inputMode:(InputMode)mode
                           key:(NSString *)key NS_SWIFT_NAME(checkIfUserPhraseExist(userPhrase:mode:key:))
{
    string unigramKey = string(key.UTF8String);
    vector<vChewing::Unigram> unigrams = [mode isEqualToString:imeModeCHT] ? gLangModelCHT.unigramsForKey(unigramKey)
                                                                           : gLangModelCHS.unigramsForKey(unigramKey);
    string userPhraseString = string(userPhrase.UTF8String);
    for (auto unigram : unigrams)
    {
        if (unigram.keyValue.value == userPhraseString)
        {
            return YES;
        }
    }
    return NO;
}

+ (BOOL)writeUserPhrase:(NSString *)userPhrase
              inputMode:(InputMode)mode
       areWeDuplicating:(BOOL)areWeDuplicating
          areWeDeleting:(BOOL)areWeDeleting
{
    if (![self checkIfUserLanguageModelFilesExist])
    {
        return NO;
    }

    // BOOL addLineBreakAtFront = NO;
    NSString *path = areWeDeleting ? [self excludedPhrasesDataPath:mode] : [self userPhrasesDataPath:mode];

    NSMutableString *currentMarkedPhrase = [NSMutableString string];
    // if (addLineBreakAtFront) {
    //     [currentMarkedPhrase appendString:@"\n"];
    // }
    [currentMarkedPhrase appendString:userPhrase];
    if (areWeDuplicating && !areWeDeleting)
    {
        // Do not use ASCII characters to comment here.
        // Otherwise, it will be scrambled by cnvHYPYtoBPMF module shipped in the vChewing Phrase Editor.
        [currentMarkedPhrase appendString:@"\t#𝙾𝚟𝚎𝚛𝚛𝚒𝚍𝚎"];
    }
    [currentMarkedPhrase appendString:@"\n"];

    NSFileHandle *writeFile = [NSFileHandle fileHandleForUpdatingAtPath:path];
    if (!writeFile)
    {
        return NO;
    }
    [writeFile seekToEndOfFile];
    NSData *data = [currentMarkedPhrase dataUsingEncoding:NSUTF8StringEncoding];
    [writeFile writeData:data];
    [writeFile closeFile];

    // We enforce the format consolidation here, since the pragma header will let the UserPhraseLM bypasses the
    // consolidating process on load.
    vChewing::LMConsolidator::ConsolidateContent([path UTF8String], false);

    //  We use FSEventStream to monitor the change of the user phrase folder,
    //  so we don't have to load data here unless FSEventStream is disabled by user.
    if (!mgrPrefs.shouldAutoReloadUserDataFiles)
    {
        [self loadUserPhrases];
    }
    return YES;
}

+ (NSString *)dataFolderPath:(bool)isDefaultFolder
{
    // 此處不能用「~」來取代當前使用者目錄名稱。不然的話，一旦輸入法被系統的沙箱干預的話，則反而會定位到沙箱目錄內。
    NSString *appSupportPath = [NSFileManager.defaultManager URLsForDirectory:NSApplicationSupportDirectory
                                                                    inDomains:NSUserDomainMask][0].path;
    NSString *userDictPath = [appSupportPath stringByAppendingPathComponent:@"vChewing"].stringByExpandingTildeInPath;
    if (mgrPrefs.userDataFolderSpecified.stringByExpandingTildeInPath == userDictPath || isDefaultFolder)
    {
        return userDictPath;
    }
    if ([mgrPrefs ifSpecifiedUserDataPathExistsInPlist])
    {
        if ([self checkIfSpecifiedUserDataFolderValid:mgrPrefs.userDataFolderSpecified.stringByExpandingTildeInPath])
        {
            return mgrPrefs.userDataFolderSpecified.stringByExpandingTildeInPath;
        }
        else
        {
            [NSUserDefaults.standardUserDefaults removeObjectForKey:@"UserDataFolderSpecified"];
        }
    }
    return userDictPath;
}

+ (NSString *)userPhrasesDataPath:(InputMode)mode;
{
    NSString *fileName = [mode isEqualToString:imeModeCHT] ? @"userdata-cht.txt" : @"userdata-chs.txt";
    return [[self dataFolderPath:false] stringByAppendingPathComponent:fileName];
}

+ (NSString *)userSymbolDataPath:(InputMode)mode;
{
    NSString *fileName =
        [mode isEqualToString:imeModeCHT] ? @"usersymbolphrases-cht.txt" : @"usersymbolphrases-chs.txt";
    return [[self dataFolderPath:false] stringByAppendingPathComponent:fileName];
}

+ (NSString *)userAssociatedPhrasesDataPath:(InputMode)mode;
{
    NSString *fileName =
        [mode isEqualToString:imeModeCHT] ? @"associatedPhrases-cht.txt" : @"associatedPhrases-chs.txt";
    return [[self dataFolderPath:false] stringByAppendingPathComponent:fileName];
}

+ (NSString *)excludedPhrasesDataPath:(InputMode)mode;
{
    NSString *fileName = [mode isEqualToString:imeModeCHT] ? @"exclude-phrases-cht.txt" : @"exclude-phrases-chs.txt";
    return [[self dataFolderPath:false] stringByAppendingPathComponent:fileName];
}

+ (NSString *)phraseReplacementDataPath:(InputMode)mode;
{
    NSString *fileName =
        [mode isEqualToString:imeModeCHT] ? @"phrases-replacement-cht.txt" : @"phrases-replacement-chs.txt";
    return [[self dataFolderPath:false] stringByAppendingPathComponent:fileName];
}

+ (vChewing::LMInstantiator *)lmCHT
{
    return &gLangModelCHT;
}

+ (vChewing::LMInstantiator *)lmCHS
{
    return &gLangModelCHS;
}

+ (vChewing::UserOverrideModel *)userOverrideModelCHT
{
    return &gUserOverrideModelCHT;
}

+ (vChewing::UserOverrideModel *)userOverrideModelCHS
{
    return &gUserOverrideModelCHS;
}

+ (void)setPhraseReplacementEnabled:(BOOL)phraseReplacementEnabled
{
    gLangModelCHT.setPhraseReplacementEnabled(phraseReplacementEnabled);
    gLangModelCHS.setPhraseReplacementEnabled(phraseReplacementEnabled);
}

+ (void)setCNSEnabled:(BOOL)cnsEnabled
{
    gLangModelCHT.setCNSEnabled(cnsEnabled);
    gLangModelCHS.setCNSEnabled(cnsEnabled);
}

+ (void)setSymbolEnabled:(BOOL)symbolEnabled
{
    gLangModelCHT.setSymbolEnabled(symbolEnabled);
    gLangModelCHS.setSymbolEnabled(symbolEnabled);
}

@end

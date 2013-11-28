/*
 * To compile:
 * clang cftest.c -o cftest -lCoreFoundation
 */

#include <CoreFoundation/CoreFoundation.h>

#define kNumKids 2
#define kNumBytesInPic 10

CFDictionaryRef CreateMyDictionary(void);
CFPropertyListRef CreateMyPropertyListFromFile(CFURLRef fileURL);

void WriteMyPropertyListToFile(CFPropertyListRef propertyList, CFURLRef fileURL);

int main (int argc, char *argv[]) {

    // Construct a complex dictionary object;
    CFPropertyListRef propertyList = CreateMyDictionary();

    printf("Creating Property List...\n");
    // Create a URL specifying the file to hold the XML data.
    CFURLRef fileURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
                           CFSTR("cftest.plist"),      // file path name
                           kCFURLPOSIXPathStyle,   // interpret as POSIX path
                           false);                 // is it a directory?
    printf("Done!\n");

    // Write the property list to the file.
    WriteMyPropertyListToFile(propertyList, fileURL);
    CFRelease(propertyList);

    // Recreate the property list from the file.
    propertyList = CreateMyPropertyListFromFile(fileURL);

    // Release objects we created.
    CFRelease(propertyList);
    CFRelease(fileURL);
    return 0;
}

CFDictionaryRef CreateMyDictionary(void) {

    // Create a dictionary that will hold the data.
    CFMutableDictionaryRef dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                       &kCFTypeDictionaryKeyCallBacks,
                                       &kCFTypeDictionaryValueCallBacks);

    /*
     Put various items into the dictionary.
     Values are retained as they are placed into the dictionary, so any values
     that are created can be released after being added to the dictionary.
    */

    CFDictionarySetValue(dict, CFSTR("Name"), CFSTR("John Doe"));

    CFDictionarySetValue(dict, CFSTR("City of Birth"), CFSTR("Springfield"));

    int yearOfBirth = 1965;
    CFNumberRef num = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &yearOfBirth);
    CFDictionarySetValue(dict, CFSTR("Year Of Birth"), num);
    CFRelease(num);

    CFStringRef kidsNames[kNumKids];
    // Define some data.
    kidsNames[0] = CFSTR("John");
    kidsNames[1] = CFSTR("Kyra");
    CFArrayRef array = CFArrayCreate(kCFAllocatorDefault,
                           (const void **)kidsNames,
                           kNumKids,
                           &kCFTypeArrayCallBacks);
    CFDictionarySetValue(dict, CFSTR("Kids Names"), array);
    CFRelease(array);

    array = CFArrayCreate(kCFAllocatorDefault, NULL, 0, &kCFTypeArrayCallBacks);
    CFDictionarySetValue(dict, CFSTR("Pets Names"), array);
    CFRelease(array);

    // Fake data to stand in for a picture of John Doe.
    const unsigned char pic[kNumBytesInPic] = {0x3c, 0x42, 0x81,
             0xa5, 0x81, 0xa5, 0x99, 0x81, 0x42, 0x3c};
    CFDataRef data = CFDataCreate(kCFAllocatorDefault, pic, kNumBytesInPic);
    CFDictionarySetValue(dict, CFSTR("Picture"), data);
    CFRelease(data);

    return dict;
}

void WriteMyPropertyListToFile(CFPropertyListRef propertyList, CFURLRef fileURL) {

    printf("Converting Property List to XML...\n");
    // Convert the property list into XML data
    CFErrorRef myError;
    CFDataRef xmlData = CFPropertyListCreateData(
                 kCFAllocatorDefault, propertyList, kCFPropertyListXMLFormat_v1_0, 0, &myError);
    // Handle any errors
     printf("Done!\n");

    printf("Writing XML Property List...\n");
    // Write the XML data to the file.
    SInt32 errorCode;
    Boolean status = CFURLWriteDataAndPropertiesToResource(
                        fileURL, xmlData, NULL, &errorCode);
    printf("Done!\n");

    if (!status) {
        // Handle the error.
    }

    printf("Releasing resources...\n");
    if (xmlData != NULL) {
        CFRelease(xmlData);
        printf("XML data released.\n");
    }
//    if (myError != NULL)
//        CFRelease(myError);
    printf("Done!\n");
}

CFPropertyListRef CreateMyPropertyListFromFile(CFURLRef fileURL) {

    // Read the XML file
    CFDataRef resourceData;
    SInt32 errorCode;
    Boolean status = CFURLCreateDataAndPropertiesFromResource(
               kCFAllocatorDefault, fileURL, &resourceData,
               NULL, NULL, &errorCode);

    if (!status) {
        // Handle the error
	printf("error: CFURLCreateDataAndPropertiesFromResource\n");
	exit(1);
    }

    // Reconstitute the dictionary using the XML data
    CFErrorRef myError;
    CFPropertyListRef propertyList = CFPropertyListCreateWithData(
                          kCFAllocatorDefault, resourceData, kCFPropertyListImmutable, NULL, &myError);

    // Handle any errors

    if (resourceData != NULL)
        CFRelease(resourceData);
//    if (myError != NULL)
//    	CFRelease(myError);

    return propertyList;
}

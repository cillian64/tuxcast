#ifndef VERSION_H
#define VERSION_H

// This is ALWAYS defined:
#define RELEASE_VERSION "0.3"
// (It is the major version this is based on - e.g. in SVN it will be the last
// release)

// These are defined for official releases:
//#define RELEASE_BINARY
//#define RELEASE_SOURCE

// These are defined for subversion checkouts
#define SUBVERSION
#define SUBVERSION_REVISION "203"
// #define SUBVERSION_BRANCH "branchname"

void version();

#endif

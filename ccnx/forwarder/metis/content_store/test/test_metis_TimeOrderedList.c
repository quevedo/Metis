/*
 * Copyright (c) 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * @author Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Runner.

#include <config.h>
#include <stdlib.h>  //  for rand()

#include "../metis_TimeOrderedList.c"

#include <LongBow/unit-test.h>

#include <parc/algol/parc_SafeMemory.h>
#include <parc/logging/parc_LogReporterTextStdout.h>

LONGBOW_TEST_RUNNER(metis_TimeOrderedList)
{
    srand(5150); // A fixed seed for the RNG for consistency.
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);

    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Static);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metis_TimeOrderedList)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metis_TimeOrderedList)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisTimeOrderedList_CreateRelease);
    LONGBOW_RUN_TEST_CASE(Global, metisTimeOrderedList_AcquireRelease);
    LONGBOW_RUN_TEST_CASE(Global, metisTimeOrderedList_AddRemove);
    LONGBOW_RUN_TEST_CASE(Global, metisTimeOrderedList_GetOldest);
    LONGBOW_RUN_TEST_CASE(Global, metisTimeOrderedList_Length);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

static MetisLogger *
_createLogger()
{
    PARCLogReporter *reporter = parcLogReporterTextStdout_Create();
    MetisLogger *logger = metisLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);
    return logger;
}

LONGBOW_TEST_CASE(Global, metisTimeOrderedList_CreateRelease)
{
    MetisLogger *logger = _createLogger();

    MetisTimeOrderedList *list = metisTimeOrderedList_Create((MetisTimeOrderList_KeyCompare *) metisContentStoreEntry_CompareExpiryTime);
    MetisLruList *lruList = metisLruList_Create();
    MetisMessage *message = metisMessage_CreateFromArray((uint8_t *) "\x00" "ehlo", 5, 111, 2, logger);

    metisMessage_SetRecommendedCacheTimeTicks(message, 100);
    metisMessage_SetExpiryTimeTicks(message, 200);
    MetisContentStoreEntry *entry = metisContentStoreEntry_Create(message, lruList);

    metisTimeOrderedList_Add(list, entry);

    metisTimeOrderedList_Release(&list);

    metisContentStoreEntry_Release(&entry);
    metisMessage_Release(&message);  // We must release the message, as it's not acquired by the TimeOrderedList.
    metisLruList_Destroy(&lruList);
    metisLogger_Release(&logger);
}

LONGBOW_TEST_CASE(Global, metisTimeOrderedList_AcquireRelease)
{
    MetisTimeOrderedList *list = metisTimeOrderedList_Create((MetisTimeOrderList_KeyCompare *) metisContentStoreEntry_CompareExpiryTime);
    MetisTimeOrderedList *ref = metisTimeOrderedList_Acquire(list);

    assertTrue(ref == list, "Expected ref and original to be the same");

    metisTimeOrderedList_Release(&list);
    metisTimeOrderedList_Release(&ref);
}

LONGBOW_TEST_CASE(Global, metisTimeOrderedList_AddRemove)
{
    MetisLogger *logger = _createLogger();
    MetisTimeOrderedList *list = metisTimeOrderedList_Create((MetisTimeOrderList_KeyCompare *) metisContentStoreEntry_CompareExpiryTime);
    MetisLruList *lruList = metisLruList_Create();

    size_t numEntries = 100;
    MetisContentStoreEntry **contentEntryList = parcMemory_AllocateAndClear(numEntries * sizeof(MetisContentStoreEntry *));

    for (int i = 1; i <= numEntries; i++) {
        MetisMessage *message = metisMessage_CreateFromArray((uint8_t *) "\x00" "ehlo", 5, 111, i, logger);

        metisMessage_SetRecommendedCacheTimeTicks(message, i % 10); // i % 10 will ensure that there are duplicate time entries.
        metisMessage_SetExpiryTimeTicks(message, i % 10);

        contentEntryList[i - 1] = metisContentStoreEntry_Create(message, lruList);
        metisTimeOrderedList_Add(list, contentEntryList[i - 1]);

        assertTrue(metisTimeOrderedList_Length(list) == i, "Got wrong TimeOrderedList object count");
    }

    for (int i = 1; i <= numEntries; i++) {
        MetisContentStoreEntry *contentEntry = contentEntryList[i - 1];
        metisTimeOrderedList_Remove(list, contentEntry);
        MetisMessage *message = metisContentStoreEntry_GetMessage(contentEntry);
        metisMessage_Release(&message);

        size_t count = metisTimeOrderedList_Length(list);
        assertTrue(count == numEntries - i, "Got wrong TimeOrderedList object count");
        metisContentStoreEntry_Release(&contentEntry);
    }

    parcMemory_Deallocate((void **) &contentEntryList);
    metisTimeOrderedList_Release(&list);
    metisLruList_Destroy(&lruList);
    metisLogger_Release(&logger);
}

LONGBOW_TEST_CASE(Global, metisTimeOrderedList_GetOldest)
{
    MetisLogger *logger = _createLogger();

    // We're going to use the ExpiryTime as the sorting comparison for GetOldest().
    MetisTimeOrderedList *list = metisTimeOrderedList_Create((MetisTimeOrderList_KeyCompare *) metisContentStoreEntry_CompareExpiryTime);

    // Add some entries, with randomly ordered ExpiryTimes.
    for (int i = 0; i < 100; i++) {
        uint64_t time = (uint64_t) rand() + 1;

        MetisMessage *message = metisMessage_CreateFromArray((uint8_t *) "\x00" "ehlo", 5, 111, i, logger);

        metisMessage_SetRecommendedCacheTimeTicks(message, 100); // constant RCT
        metisMessage_SetExpiryTimeTicks(message, time);          // random expiry time.

        MetisContentStoreEntry *entry = metisContentStoreEntry_Create(message, NULL);

        metisTimeOrderedList_Add(list, entry);

        assertTrue(metisTimeOrderedList_Length(list) == i + 1, "Got wrong TimeOrderedList object count");
    }

    // Ensure that GetOldest() always returns the oldest.

    uint64_t lastTime = 0;
    MetisContentStoreEntry *entry = NULL;
    while ((entry = metisTimeOrderedList_GetOldest(list)) != NULL) {
        MetisMessage *message = metisContentStoreEntry_GetMessage(entry);
        uint64_t messageTime = metisMessage_GetExpiryTimeTicks(message);

        // We should always retrieve a time later than the previous time we retrieved.
        assertTrue(messageTime > lastTime, "Received out of order message");

        lastTime = messageTime;
        metisTimeOrderedList_Remove(list, entry);
        metisMessage_Release(&message);
        metisContentStoreEntry_Release(&entry);
    }

    metisTimeOrderedList_Release(&list);
    metisLogger_Release(&logger);
}

LONGBOW_TEST_CASE(Global, metisTimeOrderedList_Length)
{
    MetisLogger *logger = _createLogger();

    MetisTimeOrderedList *list = metisTimeOrderedList_Create((MetisTimeOrderList_KeyCompare *) metisContentStoreEntry_CompareExpiryTime);

    // Add some entries with duplicate times to make sure that duplicate time stamps work.
    uint64_t times[] = { 1, 2, 3, 100, 100, 100, 4, 4, 3, 2, 1, 5, 6, 7, 8, 9 };
    size_t numEntriesToMake = sizeof(times) / sizeof(uint64_t);

    for (int i = 0; i < numEntriesToMake; i++) {
        MetisMessage *message = metisMessage_CreateFromArray((uint8_t *) "\x00" "ehlo", 5, 111, times[i], logger);
        metisMessage_SetExpiryTimeTicks(message, times[i]);
        MetisContentStoreEntry *entry = metisContentStoreEntry_Create(message, NULL);
        metisTimeOrderedList_Add(list, entry);
        assertTrue(metisTimeOrderedList_Length(list) == i + 1, "Got wrong TimeOrderedList object count");
    }

    // Clean up the messages we allocated.
    int i = 0;
    MetisContentStoreEntry *entry = NULL;
    while ((entry = metisTimeOrderedList_GetOldest(list)) != NULL) {
        assertTrue(metisTimeOrderedList_Length(list) == numEntriesToMake - i, "Got wrong TimeOrderedList object count");

        MetisMessage *message = metisContentStoreEntry_GetMessage(entry);
        metisTimeOrderedList_Remove(list, entry);
        metisMessage_Release(&message);
        metisContentStoreEntry_Release(&entry);
        i++;
    }

    metisTimeOrderedList_Release(&list);
    metisLogger_Release(&logger);
}

LONGBOW_TEST_FIXTURE(Static)
{
}

LONGBOW_TEST_FIXTURE_SETUP(Static)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Static)
{
    if (parcSafeMemory_ReportAllocation(STDOUT_FILENO) != 0) {
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metis_TimeOrderedList);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}


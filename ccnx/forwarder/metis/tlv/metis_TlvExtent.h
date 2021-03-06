/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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
 * @file metis_TlvExtent.h
 * @brief Defines the extent structure used to map a TLV packet
 *
 * In MetisTlvSkeleton, all the pertinent fields used by Metis are stored by their extent range in the
 * received packet buffer.  An extent is (offset, length).
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
#ifndef Metis_metis_tlv_Extent_h
#define Metis_metis_tlv_Extent_h

#include <stdint.h>
#include <stdbool.h>

/**
 * Stores the location of a field within a buffer.
 * The values are usually in host byte order.
 */
typedef struct metis_tlv_extent {
    uint16_t offset;
    uint16_t length;
} MetisTlvExtent;

/**
 * Used to detect a "not found" or "not present" condition.
 * Equal to { 0x0, 0x0 }, which is an invalid extent value.
 */
extern const MetisTlvExtent metisTlvExtent_NotFound;


/**
 * Determine if two MetisTlvExtent instances are equal.
 *
 * The following equivalence relations on non-null `MetisTlvExtent` instances are maintained:
 *
 *  * It is reflexive: for any non-null reference value x, `MetisTlvExtent_Equals(x, x)`
 *      must return true.
 *
 *  * It is symmetric: for any non-null reference values x and y,
 *    `metisTlvExtent_Equals(x, y)` must return true if and only if
 *        `metisTlvExtent_Equals(y, x)` returns true.
 *
 *  * It is transitive: for any non-null reference values x, y, and z, if
 *        `metisTlvExtent_Equals(x, y)` returns true and
 *        `metisTlvExtent_Equals(y, z)` returns true,
 *        then  `metisTlvExtent_Equals(x, z)` must return true.
 *
 *  * It is consistent: for any non-null reference values x and y, multiple
 *      invocations of `metisTlvExtent_Equals(x, y)` consistently return true or
 *      consistently return false.
 *
 *  * For any non-null reference value x, `metisTlvExtent_Equals(x, NULL)` must
 *      return false.
 *
 * @param a A pointer to a `MetisTlvExtent` instance.
 * @param b A pointer to a `MetisTlvExtent` instance.
 * @return true if the two `MetisTlvExtent` instances are equal.
 *
 * Example:
 * @code
 * {
 *    MetisTlvExtent *a = { .offset = 5, .length = 7 };
 *    MetisTlvExtent *b = { .offset = 5, .length = 8 };
 *
 *    if (metisTlvExtent_Equals(a, b)) {
 *        // true
 *    } else {
 *        // false (this is the block executed)
 *    }
 * }
 * @endcode
 */
bool metisTlvExtent_Equals(const MetisTlvExtent *a, const MetisTlvExtent *b);
#endif // Metis_metis_tlv_Extent_h

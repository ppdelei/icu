// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#if !UCONFIG_NO_FORMATTING
#ifndef __NUMBERRANGEFORMATTER_H__
#define __NUMBERRANGEFORMATTER_H__

#include <atomic>
#include "unicode/appendable.h"
#include "unicode/fieldpos.h"
#include "unicode/formattedvalue.h"
#include "unicode/fpositer.h"
#include "unicode/numberformatter.h"

#ifndef U_HIDE_DRAFT_API

/**
 * \file
 * \brief C++ API: Library for localized formatting of number, currency, and unit ranges.
 *
 * The main entrypoint to the formatting of ranges of numbers, including currencies and other units of measurement.
 * <p>
 * Usage example:
 * <p>
 * <pre>
 * NumberRangeFormatter::with()
 *     .identityFallback(UNUM_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE)
 *     .numberFormatterFirst(NumberFormatter::with().adoptUnit(MeasureUnit::createMeter()))
 *     .numberFormatterSecond(NumberFormatter::with().adoptUnit(MeasureUnit::createKilometer()))
 *     .locale("en-GB")
 *     .formatRange(750, 1.2, status)
 *     .toString(status);
 * // => "750 m - 1.2 km"
 * </pre>
 * <p>
 * Like NumberFormatter, NumberRangeFormatter instances are immutable and thread-safe. This API is based on the
 * <em>fluent</em> design pattern popularized by libraries such as Google's Guava.
 *
 * @author Shane Carr
 */


/**
 * Defines how to merge fields that are identical across the range sign.
 *
 * @draft ICU 63
 */
typedef enum UNumberRangeCollapse {
    /**
     * Use locale data and heuristics to determine how much of the string to collapse. Could end up collapsing none,
     * some, or all repeated pieces in a locale-sensitive way.
     *
     * The heuristics used for this option are subject to change over time.
     *
     * @draft ICU 63
     */
    UNUM_RANGE_COLLAPSE_AUTO,

    /**
     * Do not collapse any part of the number. Example: "3.2 thousand kilograms – 5.3 thousand kilograms"
     *
     * @draft ICU 63
     */
    UNUM_RANGE_COLLAPSE_NONE,

    /**
     * Collapse the unit part of the number, but not the notation, if present. Example: "3.2 thousand – 5.3 thousand
     * kilograms"
     *
     * @draft ICU 63
     */
    UNUM_RANGE_COLLAPSE_UNIT,

    /**
     * Collapse any field that is equal across the range sign. May introduce ambiguity on the magnitude of the
     * number. Example: "3.2 – 5.3 thousand kilograms"
     *
     * @draft ICU 63
     */
    UNUM_RANGE_COLLAPSE_ALL
} UNumberRangeCollapse;

/**
 * Defines the behavior when the two numbers in the range are identical after rounding. To programmatically detect
 * when the identity fallback is used, compare the lower and upper BigDecimals via FormattedNumber.
 *
 * @draft ICU 63
 * @see NumberRangeFormatter
 */
typedef enum UNumberRangeIdentityFallback {
    /**
     * Show the number as a single value rather than a range. Example: "$5"
     *
     * @draft ICU 63
     */
    UNUM_IDENTITY_FALLBACK_SINGLE_VALUE,

    /**
     * Show the number using a locale-sensitive approximation pattern. If the numbers were the same before rounding,
     * show the single value. Example: "~$5" or "$5"
     *
     * @draft ICU 63
     */
    UNUM_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE,

    /**
     * Show the number using a locale-sensitive approximation pattern. Use the range pattern always, even if the
     * inputs are the same. Example: "~$5"
     *
     * @draft ICU 63
     */
    UNUM_IDENTITY_FALLBACK_APPROXIMATELY,

    /**
     * Show the number as the range of two equal values. Use the range pattern always, even if the inputs are the
     * same. Example (with RangeCollapse.NONE): "$5 – $5"
     *
     * @draft ICU 63
     */
    UNUM_IDENTITY_FALLBACK_RANGE
} UNumberRangeIdentityFallback;

/**
 * Used in the result class FormattedNumberRange to indicate to the user whether the numbers formatted in the range
 * were equal or not, and whether or not the identity fallback was applied.
 *
 * @draft ICU 63
 * @see NumberRangeFormatter
 */
typedef enum UNumberRangeIdentityResult {
    /**
     * Used to indicate that the two numbers in the range were equal, even before any rounding rules were applied.
     *
     * @draft ICU 63
     * @see NumberRangeFormatter
     */
    UNUM_IDENTITY_RESULT_EQUAL_BEFORE_ROUNDING,

    /**
     * Used to indicate that the two numbers in the range were equal, but only after rounding rules were applied.
     *
     * @draft ICU 63
     * @see NumberRangeFormatter
     */
    UNUM_IDENTITY_RESULT_EQUAL_AFTER_ROUNDING,

    /**
     * Used to indicate that the two numbers in the range were not equal, even after rounding rules were applied.
     *
     * @draft ICU 63
     * @see NumberRangeFormatter
     */
    UNUM_IDENTITY_RESULT_NOT_EQUAL,

#ifndef U_HIDE_INTERNAL_API
    /**
     * The number of entries in this enum.
     * @internal
     */
    UNUM_IDENTITY_RESULT_COUNT
#endif

} UNumberRangeIdentityResult;

U_NAMESPACE_BEGIN

namespace number {  // icu::number

// Forward declarations:
class UnlocalizedNumberRangeFormatter;
class LocalizedNumberRangeFormatter;
class FormattedNumberRange;

namespace impl {

// Forward declarations:
struct RangeMacroProps;
class DecimalQuantity;
struct UFormattedNumberRangeData;
class NumberRangeFormatterImpl;

} // namespace impl

/**
 * \cond
 * Export an explicit template instantiation. See datefmt.h
 * (When building DLLs for Windows this is required.)
 */
#if U_PLATFORM == U_PF_WINDOWS && !defined(U_IN_DOXYGEN)
} // namespace icu::number
U_NAMESPACE_END

template struct U_I18N_API std::atomic< U_NAMESPACE_QUALIFIER number::impl::NumberRangeFormatterImpl*>;

U_NAMESPACE_BEGIN
namespace number {  // icu::number
#endif
/** \endcond */

// Other helper classes would go here, but there are none.

namespace impl {  // icu::number::impl

// Do not enclose entire MacroProps with #ifndef U_HIDE_INTERNAL_API, needed for a protected field
/** @internal */
struct U_I18N_API RangeMacroProps : public UMemory {
    /** @internal */
    UnlocalizedNumberFormatter formatter1; // = NumberFormatter::with();

    /** @internal */
    UnlocalizedNumberFormatter formatter2; // = NumberFormatter::with();

    /** @internal */
    bool singleFormatter = true;

    /** @internal */
    UNumberRangeCollapse collapse = UNUM_RANGE_COLLAPSE_AUTO;

    /** @internal */
    UNumberRangeIdentityFallback identityFallback = UNUM_IDENTITY_FALLBACK_APPROXIMATELY;

    /** @internal */
    Locale locale;

    // NOTE: Uses default copy and move constructors.

    /**
     * Check all members for errors.
     * @internal
     */
    bool copyErrorTo(UErrorCode &status) const {
        return formatter1.copyErrorTo(status) || formatter2.copyErrorTo(status);
    }
};

} // namespace impl

/**
 * An abstract base class for specifying settings related to number formatting. This class is implemented by
 * {@link UnlocalizedNumberRangeFormatter} and {@link LocalizedNumberRangeFormatter}. This class is not intended for
 * public subclassing.
 */
template<typename Derived>
class U_I18N_API NumberRangeFormatterSettings {
  public:
    /**
     * Sets the NumberFormatter instance to use for the numbers in the range. The same formatter is applied to both
     * sides of the range.
     * <p>
     * The NumberFormatter instances must not have a locale applied yet; the locale specified on the
     * NumberRangeFormatter will be used.
     *
     * @param formatter
     *            The formatter to use for both numbers in the range.
     * @return The fluent chain.
     * @draft ICU 63
     */
    Derived numberFormatterBoth(const UnlocalizedNumberFormatter &formatter) const &;

    /**
     * Overload of numberFormatterBoth() for use on an rvalue reference.
     *
     * @param formatter
     *            The formatter to use for both numbers in the range.
     * @return The fluent chain.
     * @see #numberFormatterBoth
     * @draft ICU 63
     */
    Derived numberFormatterBoth(const UnlocalizedNumberFormatter &formatter) &&;

    /**
     * Overload of numberFormatterBoth() for use on an rvalue reference.
     *
     * @param formatter
     *            The formatter to use for both numbers in the range.
     * @return The fluent chain.
     * @see #numberFormatterBoth
     * @draft ICU 63
     */
    Derived numberFormatterBoth(UnlocalizedNumberFormatter &&formatter) const &;

    /**
     * Overload of numberFormatterBoth() for use on an rvalue reference.
     *
     * @param formatter
     *            The formatter to use for both numbers in the range.
     * @return The fluent chain.
     * @see #numberFormatterBoth
     * @draft ICU 63
     */
    Derived numberFormatterBoth(UnlocalizedNumberFormatter &&formatter) &&;

    /**
     * Sets the NumberFormatter instance to use for the first number in the range.
     * <p>
     * The NumberFormatter instances must not have a locale applied yet; the locale specified on the
     * NumberRangeFormatter will be used.
     *
     * @param formatterFirst
     *            The formatter to use for the first number in the range.
     * @return The fluent chain.
     * @draft ICU 63
     */
    Derived numberFormatterFirst(const UnlocalizedNumberFormatter &formatterFirst) const &;

    /**
     * Overload of numberFormatterFirst() for use on an rvalue reference.
     *
     * @param formatterFirst
     *            The formatter to use for the first number in the range.
     * @return The fluent chain.
     * @see #numberFormatterFirst
     * @draft ICU 63
     */
    Derived numberFormatterFirst(const UnlocalizedNumberFormatter &formatterFirst) &&;

    /**
     * Overload of numberFormatterFirst() for use on an rvalue reference.
     *
     * @param formatterFirst
     *            The formatter to use for the first number in the range.
     * @return The fluent chain.
     * @see #numberFormatterFirst
     * @draft ICU 63
     */
    Derived numberFormatterFirst(UnlocalizedNumberFormatter &&formatterFirst) const &;

    /**
     * Overload of numberFormatterFirst() for use on an rvalue reference.
     *
     * @param formatterFirst
     *            The formatter to use for the first number in the range.
     * @return The fluent chain.
     * @see #numberFormatterFirst
     * @draft ICU 63
     */
    Derived numberFormatterFirst(UnlocalizedNumberFormatter &&formatterFirst) &&;

    /**
     * Sets the NumberFormatter instance to use for the second number in the range.
     * <p>
     * The NumberFormatter instances must not have a locale applied yet; the locale specified on the
     * NumberRangeFormatter will be used.
     *
     * @param formatterSecond
     *            The formatter to use for the second number in the range.
     * @return The fluent chain.
     * @draft ICU 63
     */
    Derived numberFormatterSecond(const UnlocalizedNumberFormatter &formatterSecond) const &;

    /**
     * Overload of numberFormatterSecond() for use on an rvalue reference.
     *
     * @param formatterSecond
     *            The formatter to use for the second number in the range.
     * @return The fluent chain.
     * @see #numberFormatterSecond
     * @draft ICU 63
     */
    Derived numberFormatterSecond(const UnlocalizedNumberFormatter &formatterSecond) &&;

    /**
     * Overload of numberFormatterSecond() for use on an rvalue reference.
     *
     * @param formatterSecond
     *            The formatter to use for the second number in the range.
     * @return The fluent chain.
     * @see #numberFormatterSecond
     * @draft ICU 63
     */
    Derived numberFormatterSecond(UnlocalizedNumberFormatter &&formatterSecond) const &;

    /**
     * Overload of numberFormatterSecond() for use on an rvalue reference.
     *
     * @param formatterSecond
     *            The formatter to use for the second number in the range.
     * @return The fluent chain.
     * @see #numberFormatterSecond
     * @draft ICU 63
     */
    Derived numberFormatterSecond(UnlocalizedNumberFormatter &&formatterSecond) &&;

    /**
     * Sets the aggressiveness of "collapsing" fields across the range separator. Possible values:
     * <p>
     * <ul>
     * <li>ALL: "3-5K miles"</li>
     * <li>UNIT: "3K - 5K miles"</li>
     * <li>NONE: "3K miles - 5K miles"</li>
     * <li>AUTO: usually UNIT or NONE, depending on the locale and formatter settings</li>
     * </ul>
     * <p>
     * The default value is AUTO.
     *
     * @param collapse
     *            The collapsing strategy to use for this range.
     * @return The fluent chain.
     * @draft ICU 63
     */
    Derived collapse(UNumberRangeCollapse collapse) const &;

    /**
     * Overload of collapse() for use on an rvalue reference.
     *
     * @param collapse
     *            The collapsing strategy to use for this range.
     * @return The fluent chain.
     * @see #collapse
     * @draft ICU 63
     */
    Derived collapse(UNumberRangeCollapse collapse) &&;

    /**
     * Sets the behavior when the two sides of the range are the same. This could happen if the same two numbers are
     * passed to the formatRange function, or if different numbers are passed to the function but they become the same
     * after rounding rules are applied. Possible values:
     * <p>
     * <ul>
     * <li>SINGLE_VALUE: "5 miles"</li>
     * <li>APPROXIMATELY_OR_SINGLE_VALUE: "~5 miles" or "5 miles", depending on whether the number was the same before
     * rounding was applied</li>
     * <li>APPROXIMATELY: "~5 miles"</li>
     * <li>RANGE: "5-5 miles" (with collapse=UNIT)</li>
     * </ul>
     * <p>
     * The default value is APPROXIMATELY.
     *
     * @param identityFallback
     *            The strategy to use when formatting two numbers that end up being the same.
     * @return The fluent chain.
     * @draft ICU 63
     */
    Derived identityFallback(UNumberRangeIdentityFallback identityFallback) const &;

    /**
     * Overload of identityFallback() for use on an rvalue reference.
     *
     * @param identityFallback
     *            The strategy to use when formatting two numbers that end up being the same.
     * @return The fluent chain.
     * @see #identityFallback
     * @draft ICU 63
     */
    Derived identityFallback(UNumberRangeIdentityFallback identityFallback) &&;

    /**
     * Returns the current (Un)LocalizedNumberRangeFormatter as a LocalPointer
     * wrapping a heap-allocated copy of the current object.
     *
     * This is equivalent to new-ing the move constructor with a value object
     * as the argument.
     *
     * @return A wrapped (Un)LocalizedNumberRangeFormatter pointer, or a wrapped
     *         nullptr on failure.
     * @draft ICU 64
     */
    LocalPointer<Derived> clone() const &;

    /**
     * Overload of clone for use on an rvalue reference.
     *
     * @return A wrapped (Un)LocalizedNumberRangeFormatter pointer, or a wrapped
     *         nullptr on failure.
     * @draft ICU 64
     */
    LocalPointer<Derived> clone() &&;

    /**
     * Sets the UErrorCode if an error occurred in the fluent chain.
     * Preserves older error codes in the outErrorCode.
     * @return TRUE if U_FAILURE(outErrorCode)
     * @draft ICU 63
     */
    UBool copyErrorTo(UErrorCode &outErrorCode) const {
        if (U_FAILURE(outErrorCode)) {
            // Do not overwrite the older error code
            return TRUE;
        }
        fMacros.copyErrorTo(outErrorCode);
        return U_FAILURE(outErrorCode);
    };

    // NOTE: Uses default copy and move constructors.

  private:
    impl::RangeMacroProps fMacros;

    // Don't construct me directly!  Use (Un)LocalizedNumberFormatter.
    NumberRangeFormatterSettings() = default;

    friend class LocalizedNumberRangeFormatter;
    friend class UnlocalizedNumberRangeFormatter;
};

/**
 * A NumberRangeFormatter that does not yet have a locale. In order to format, a locale must be specified.
 *
 * @see NumberRangeFormatter
 * @draft ICU 63
 */
class U_I18N_API UnlocalizedNumberRangeFormatter
        : public NumberRangeFormatterSettings<UnlocalizedNumberRangeFormatter>, public UMemory {

  public:
    /**
     * Associate the given locale with the number range formatter. The locale is used for picking the
     * appropriate symbols, formats, and other data for number display.
     *
     * @param locale
     *            The locale to use when loading data for number formatting.
     * @return The fluent chain.
     * @draft ICU 63
     */
    LocalizedNumberRangeFormatter locale(const icu::Locale &locale) const &;

    /**
     * Overload of locale() for use on an rvalue reference.
     *
     * @param locale
     *            The locale to use when loading data for number formatting.
     * @return The fluent chain.
     * @see #locale
     * @draft ICU 63
     */
    LocalizedNumberRangeFormatter locale(const icu::Locale &locale) &&;

    /**
     * Default constructor: puts the formatter into a valid but undefined state.
     *
     * @draft ICU 63
     */
    UnlocalizedNumberRangeFormatter() = default;

    /**
     * Returns a copy of this UnlocalizedNumberRangeFormatter.
     * @draft ICU 63
     */
    UnlocalizedNumberRangeFormatter(const UnlocalizedNumberRangeFormatter &other);

    /**
     * Move constructor:
     * The source UnlocalizedNumberRangeFormatter will be left in a valid but undefined state.
     * @draft ICU 63
     */
    UnlocalizedNumberRangeFormatter(UnlocalizedNumberRangeFormatter&& src) U_NOEXCEPT;

    /**
     * Copy assignment operator.
     * @draft ICU 63
     */
    UnlocalizedNumberRangeFormatter& operator=(const UnlocalizedNumberRangeFormatter& other);

    /**
     * Move assignment operator:
     * The source UnlocalizedNumberRangeFormatter will be left in a valid but undefined state.
     * @draft ICU 63
     */
    UnlocalizedNumberRangeFormatter& operator=(UnlocalizedNumberRangeFormatter&& src) U_NOEXCEPT;

  private:
    explicit UnlocalizedNumberRangeFormatter(
            const NumberRangeFormatterSettings<UnlocalizedNumberRangeFormatter>& other);

    explicit UnlocalizedNumberRangeFormatter(
            NumberRangeFormatterSettings<UnlocalizedNumberRangeFormatter>&& src) U_NOEXCEPT;

    // To give the fluent setters access to this class's constructor:
    friend class NumberRangeFormatterSettings<UnlocalizedNumberRangeFormatter>;

    // To give NumberRangeFormatter::with() access to this class's constructor:
    friend class NumberRangeFormatter;
};

/**
 * A NumberRangeFormatter that has a locale associated with it; this means .formatRange() methods are available.
 *
 * @see NumberFormatter
 * @draft ICU 63
 */
class U_I18N_API LocalizedNumberRangeFormatter
        : public NumberRangeFormatterSettings<LocalizedNumberRangeFormatter>, public UMemory {
  public:
    /**
     * Format the given Formattables to a string using the settings specified in the NumberRangeFormatter fluent setting
     * chain.
     *
     * @param first
     *            The first number in the range, usually to the left in LTR locales.
     * @param second
     *            The second number in the range, usually to the right in LTR locales.
     * @param status
     *            Set if an error occurs while formatting.
     * @return A FormattedNumberRange object; call .toString() to get the string.
     * @draft ICU 63
     */
    FormattedNumberRange formatFormattableRange(
        const Formattable& first, const Formattable& second, UErrorCode& status) const;

    /**
     * Default constructor: puts the formatter into a valid but undefined state.
     *
     * @draft ICU 63
     */
    LocalizedNumberRangeFormatter() = default;

    /**
     * Returns a copy of this LocalizedNumberRangeFormatter.
     * @draft ICU 63
     */
    LocalizedNumberRangeFormatter(const LocalizedNumberRangeFormatter &other);

    /**
     * Move constructor:
     * The source LocalizedNumberRangeFormatter will be left in a valid but undefined state.
     * @draft ICU 63
     */
    LocalizedNumberRangeFormatter(LocalizedNumberRangeFormatter&& src) U_NOEXCEPT;

    /**
     * Copy assignment operator.
     * @draft ICU 63
     */
    LocalizedNumberRangeFormatter& operator=(const LocalizedNumberRangeFormatter& other);

    /**
     * Move assignment operator:
     * The source LocalizedNumberRangeFormatter will be left in a valid but undefined state.
     * @draft ICU 63
     */
    LocalizedNumberRangeFormatter& operator=(LocalizedNumberRangeFormatter&& src) U_NOEXCEPT;

#ifndef U_HIDE_INTERNAL_API

    /**
     * @param results
     *            The results object. This method will mutate it to save the results.
     * @param equalBeforeRounding
     *            Whether the number was equal before copying it into a DecimalQuantity.
     *            Used for determining the identity fallback behavior.
     * @param status
     *            Set if an error occurs while formatting.
     * @internal
     */
    void formatImpl(impl::UFormattedNumberRangeData& results, bool equalBeforeRounding,
                    UErrorCode& status) const;

#endif

    /**
     * Destruct this LocalizedNumberRangeFormatter, cleaning up any memory it might own.
     * @draft ICU 63
     */
    ~LocalizedNumberRangeFormatter();

  private:
    std::atomic<impl::NumberRangeFormatterImpl*> fAtomicFormatter = {};

    const impl::NumberRangeFormatterImpl* getFormatter(UErrorCode& stauts) const;

    explicit LocalizedNumberRangeFormatter(
        const NumberRangeFormatterSettings<LocalizedNumberRangeFormatter>& other);

    explicit LocalizedNumberRangeFormatter(
        NumberRangeFormatterSettings<LocalizedNumberRangeFormatter>&& src) U_NOEXCEPT;

    LocalizedNumberRangeFormatter(const impl::RangeMacroProps &macros, const Locale &locale);

    LocalizedNumberRangeFormatter(impl::RangeMacroProps &&macros, const Locale &locale);

    void clear();

    // To give the fluent setters access to this class's constructor:
    friend class NumberRangeFormatterSettings<UnlocalizedNumberRangeFormatter>;
    friend class NumberRangeFormatterSettings<LocalizedNumberRangeFormatter>;

    // To give UnlocalizedNumberRangeFormatter::locale() access to this class's constructor:
    friend class UnlocalizedNumberRangeFormatter;
};

/**
 * The result of a number range formatting operation. This class allows the result to be exported in several data types,
 * including a UnicodeString and a FieldPositionIterator.
 *
 * @draft ICU 63
 */
class U_I18N_API FormattedNumberRange : public UMemory, public FormattedValue {
  public:
    /**
     * @copydoc FormattedValue::toString()
     */
    UnicodeString toString(UErrorCode& status) const U_OVERRIDE;

    /**
     * @copydoc FormattedValue::toTempString()
     */
    UnicodeString toTempString(UErrorCode& status) const U_OVERRIDE;

    /**
     * @copydoc FormattedValue::appendTo()
     */
    Appendable &appendTo(Appendable &appendable, UErrorCode& status) const U_OVERRIDE;

    /**
     * @copydoc FormattedValue::nextPosition()
     */
    UBool nextPosition(ConstrainedFieldPosition& cfpos, UErrorCode& status) const U_OVERRIDE;

    /**
     * Determines the start (inclusive) and end (exclusive) indices of the next occurrence of the given
     * <em>field</em> in the output string. This allows you to determine the locations of, for example,
     * the integer part, fraction part, or symbols.
     *
     * If both sides of the range have the same field, the field will occur twice, once before the
     * range separator and once after the range separator, if applicable.
     *
     * If a field occurs just once, calling this method will find that occurrence and return it. If a
     * field occurs multiple times, this method may be called repeatedly with the following pattern:
     *
     * <pre>
     * FieldPosition fpos(UNUM_INTEGER_FIELD);
     * while (formattedNumberRange.nextFieldPosition(fpos, status)) {
     *   // do something with fpos.
     * }
     * </pre>
     *
     * This method is useful if you know which field to query. If you want all available field position
     * information, use #getAllFieldPositions().
     *
     * @param fieldPosition
     *            Input+output variable. See {@link FormattedNumber#nextFieldPosition}.
     * @param status
     *            Set if an error occurs while populating the FieldPosition.
     * @return TRUE if a new occurrence of the field was found; FALSE otherwise.
     * @draft ICU 63
     * @see UNumberFormatFields
     */
    UBool nextFieldPosition(FieldPosition& fieldPosition, UErrorCode& status) const;

    /**
     * Export the formatted number range to a FieldPositionIterator. This allows you to determine which characters in
     * the output string correspond to which <em>fields</em>, such as the integer part, fraction part, and sign.
     *
     * If information on only one field is needed, use #nextFieldPosition() instead.
     *
     * @param iterator
     *            The FieldPositionIterator to populate with all of the fields present in the formatted number.
     * @param status
     *            Set if an error occurs while populating the FieldPositionIterator.
     * @draft ICU 63
     * @see UNumberFormatFields
     */
    void getAllFieldPositions(FieldPositionIterator &iterator, UErrorCode &status) const;

    /**
     * Export the first formatted number as a decimal number. This endpoint
     * is useful for obtaining the exact number being printed after scaling
     * and rounding have been applied by the number range formatting pipeline.
     * 
     * The syntax of the unformatted number is a "numeric string"
     * as defined in the Decimal Arithmetic Specification, available at
     * http://speleotrove.com/decimal
     *
     * @return A decimal representation of the first formatted number.
     * @draft ICU 63
     * @see NumberRangeFormatter
     * @see #getSecondDecimal
     */
    UnicodeString getFirstDecimal(UErrorCode& status) const;

    /**
     * Export the second formatted number as a decimal number. This endpoint
     * is useful for obtaining the exact number being printed after scaling
     * and rounding have been applied by the number range formatting pipeline.
     * 
     * The syntax of the unformatted number is a "numeric string"
     * as defined in the Decimal Arithmetic Specification, available at
     * http://speleotrove.com/decimal
     *
     * @return A decimal representation of the second formatted number.
     * @draft ICU 63
     * @see NumberRangeFormatter
     * @see #getFirstDecimal
     */
    UnicodeString getSecondDecimal(UErrorCode& status) const;

    /**
     * Returns whether the pair of numbers was successfully formatted as a range or whether an identity fallback was
     * used. For example, if the first and second number were the same either before or after rounding occurred, an
     * identity fallback was used.
     *
     * @return An indication the resulting identity situation in the formatted number range.
     * @draft ICU 63
     * @see UNumberRangeIdentityFallback
     */
    UNumberRangeIdentityResult getIdentityResult(UErrorCode& status) const;

    /**
     * Copying not supported; use move constructor instead.
     */
    FormattedNumberRange(const FormattedNumberRange&) = delete;

    /**
     * Copying not supported; use move assignment instead.
     */
    FormattedNumberRange& operator=(const FormattedNumberRange&) = delete;

    /**
     * Move constructor:
     * Leaves the source FormattedNumberRange in an undefined state.
     * @draft ICU 63
     */
    FormattedNumberRange(FormattedNumberRange&& src) U_NOEXCEPT;

    /**
     * Move assignment:
     * Leaves the source FormattedNumberRange in an undefined state.
     * @draft ICU 63
     */
    FormattedNumberRange& operator=(FormattedNumberRange&& src) U_NOEXCEPT;

    /**
     * Destruct an instance of FormattedNumberRange, cleaning up any memory it might own.
     * @draft ICU 63
     */
    ~FormattedNumberRange();

  private:
    // Can't use LocalPointer because UFormattedNumberRangeData is forward-declared
    const impl::UFormattedNumberRangeData *fResults;

    // Error code for the terminal methods
    UErrorCode fErrorCode;

    /**
     * Internal constructor from data type. Adopts the data pointer.
     * @internal
     */
    explicit FormattedNumberRange(impl::UFormattedNumberRangeData *results)
        : fResults(results), fErrorCode(U_ZERO_ERROR) {};

    explicit FormattedNumberRange(UErrorCode errorCode)
        : fResults(nullptr), fErrorCode(errorCode) {};

    void getAllFieldPositionsImpl(FieldPositionIteratorHandler& fpih, UErrorCode& status) const;

    // To give LocalizedNumberRangeFormatter format methods access to this class's constructor:
    friend class LocalizedNumberRangeFormatter;
};

/**
 * See the main description in numberrangeformatter.h for documentation and examples.
 *
 * @draft ICU 63
 */
class U_I18N_API NumberRangeFormatter final {
  public:
    /**
     * Call this method at the beginning of a NumberRangeFormatter fluent chain in which the locale is not currently
     * known at the call site.
     *
     * @return An {@link UnlocalizedNumberRangeFormatter}, to be used for chaining.
     * @draft ICU 63
     */
    static UnlocalizedNumberRangeFormatter with();

    /**
     * Call this method at the beginning of a NumberRangeFormatter fluent chain in which the locale is known at the call
     * site.
     *
     * @param locale
     *            The locale from which to load formats and symbols for number range formatting.
     * @return A {@link LocalizedNumberRangeFormatter}, to be used for chaining.
     * @draft ICU 63
     */
    static LocalizedNumberRangeFormatter withLocale(const Locale &locale);

    /**
     * Use factory methods instead of the constructor to create a NumberFormatter.
     */
    NumberRangeFormatter() = delete;
};

}  // namespace number
U_NAMESPACE_END

#endif  // U_HIDE_DRAFT_API

#endif // __NUMBERRANGEFORMATTER_H__

#endif /* #if !UCONFIG_NO_FORMATTING */

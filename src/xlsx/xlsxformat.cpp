/****************************************************************************
** Copyright (c) 2013 Debao Zhang <hello@debao.me>
** All right reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/
#include "xlsxformat.h"
#include "xlsxformat_p.h"
#include <QDataStream>
#include <QRegularExpression>
#include <QDebug>

QT_BEGIN_NAMESPACE_XLSX

FormatPrivate::FormatPrivate()
    : dirty(true)
    , font_dirty(true), font_index_valid(false), font_index(-1)
    , fill_dirty(true), fill_index_valid(false), fill_index(-1)
    , border_dirty(true), border_index_valid(false), border_index(-1)
    , xf_index(-1), xf_indexValid(false)
    , is_dxf_fomat(false), dxf_index(-1), dxf_indexValid(false)
    , theme(0)
{
}

FormatPrivate::FormatPrivate(const FormatPrivate &other)
    : QSharedData(other)
    , dirty(other.dirty), formatKey(other.formatKey)
    , font_dirty(other.font_dirty), font_index_valid(other.font_index_valid), font_key(other.font_key), font_index(other.font_index)
    , fill_dirty(other.fill_dirty), fill_index_valid(other.fill_index_valid), fill_key(other.fill_key), fill_index(other.fill_index)
    , border_dirty(other.border_dirty), border_index_valid(other.border_index_valid), border_key(other.border_key), border_index(other.border_index)
    , xf_index(other.xf_index), xf_indexValid(other.xf_indexValid)
    , is_dxf_fomat(other.is_dxf_fomat), dxf_index(other.dxf_index), dxf_indexValid(other.dxf_indexValid)
    , theme(other.theme)
    , property(other.property)
{

}

FormatPrivate::~FormatPrivate()
{

}

/*!
 * \class Format
 * \inmodule QtXlsx
 * \brief Providing the methods and properties that are available for formatting cells in Excel.
 */


/*!
 *  Creates a new format.
 */
Format::Format() :
    d(new FormatPrivate)
{

}

/*!
   Creates a new format with the same attributes as the \a other format.
 */
Format::Format(const Format &other)
    :d(other.d)
{

}

/*!
   Assigns the \a other format to this format, and returns a
   reference to this format.
 */
Format &Format::operator =(const Format &other)
{
    d = other.d;
    return *this;
}

/*!
 * Destroys this format.
 */
Format::~Format()
{
}

/*!
 * Returns the number format identifier.
 */
int Format::numberFormatIndex() const
{
    return intProperty(FormatPrivate::P_NumFmt_Id);
}

/*!
 * Set the number format identifier. The \a format
 * must be a valid built-in number format identifier
 * or the identifier of a custom number format.
 */
void Format::setNumberFormatIndex(int format)
{
    setProperty(FormatPrivate::P_NumFmt_Id, format);
    clearProperty(FormatPrivate::P_NumFmt_FormatCode);
}

/*!
 * Returns the number format string.
 * \note for built-in number formats, this may
 * return an empty string.
 */
QString Format::numberFormat() const
{
    return stringProperty(FormatPrivate::P_NumFmt_FormatCode);
}

/*!
 * Set number \a format.
 * http://office.microsoft.com/en-001/excel-help/create-a-custom-number-format-HP010342372.aspx
 */
void Format::setNumberFormat(const QString &format)
{
    if (format.isEmpty())
        return;
    setProperty(FormatPrivate::P_NumFmt_FormatCode, format);
    clearProperty(FormatPrivate::P_NumFmt_Id); //numFmt id must be re-generated.
}

/*!
 * Returns whether the number format is probably a dateTime or not
 */
bool Format::isDateTimeFormat() const
{
    if (hasProperty(FormatPrivate::P_NumFmt_FormatCode)) {
        //Custom numFmt, so
        //Gauss from the number string
        QString formatCode = numberFormat();
        formatCode.remove(QRegularExpression(QStringLiteral("\\[(Green|White|Blue|Magenta|Yellow|Cyan|Red)\\]")));
        if (formatCode.contains(QRegularExpression(QStringLiteral("[dmhys]"))))
            return true;
    } else if (hasProperty(FormatPrivate::P_NumFmt_Id)){
        //Non-custom numFmt
        int idx = numberFormatIndex();

        //Is built-in date time number id?
        if ((idx >= 15 && idx <= 22) || (idx >= 45 && idx <= 47))
            return true;
    }

    return false;
}

/*!
 * Set a custom num \a format with the given \a id.
 */
void Format::setNumberFormat(int id, const QString &format)
{
    setProperty(FormatPrivate::P_NumFmt_Id, id);
    setProperty(FormatPrivate::P_NumFmt_FormatCode, format);
}

/*!
 * Return the size of the font in points.
 */
int Format::fontSize() const
{
    return intProperty(FormatPrivate::P_Font_Size);
}

/*!
 * Set the \a size of the font in points.
 */
void Format::setFontSize(int size)
{
    setProperty(FormatPrivate::P_Font_Size, size);
}

/*!
 * Return whether the font is italic.
 */
bool Format::fontItalic() const
{
    return boolProperty(FormatPrivate::P_Font_Italic);
}

/*!
 * Turn on/off the italic font.
 */
void Format::setFontItalic(bool italic)
{
    setProperty(FormatPrivate::P_Font_Italic, italic);
}

/*!
 * Return whether the font is strikeout.
 */
bool Format::fontStrikeOut() const
{
    return boolProperty(FormatPrivate::P_Font_StrikeOut);
}

/*!
 * Turn on/off the strikeOut font.
 */
void Format::setFontStrikeOut(bool strikeOut)
{
    setProperty(FormatPrivate::P_Font_StrikeOut, strikeOut);
}

/*!
 * Return the color of the font.
 */
QColor Format::fontColor() const
{
    if (hasProperty(FormatPrivate::P_Font_Color))
        return colorProperty(FormatPrivate::P_Font_Color);

    if (hasProperty(FormatPrivate::P_Font_ThemeColor)) {
        //!Todo, get the real color from the theme{1}.xml file
        //The same is ture for border and fill colord
        return QColor();
    }
    return QColor();
}

/*!
 * Set the \a color of the font.
 */
void Format::setFontColor(const QColor &color)
{
    setProperty(FormatPrivate::P_Font_Color, color);
}

/*!
 * Return whether the font is bold.
 */
bool Format::fontBold() const
{
    return boolProperty(FormatPrivate::P_Font_Bold);
}

/*!
 * Turn on/off the bold font.
 */
void Format::setFontBold(bool bold)
{
    setProperty(FormatPrivate::P_Font_Bold, bold);
}

/*!
 * Return the script style of the font.
 */
Format::FontScript Format::fontScript() const
{
    return static_cast<Format::FontScript>(intProperty(FormatPrivate::P_Font_Script));
}

/*!
 * Set the script style of the font.
 */
void Format::setFontScript(FontScript script)
{
    setProperty(FormatPrivate::P_Font_Script, script);
}

/*!
 * Return the underline style of the font.
 */
Format::FontUnderline Format::fontUnderline() const
{
    return static_cast<Format::FontUnderline>(intProperty(FormatPrivate::P_Font_Underline));
}

/*!
 * Set the underline style of the font.
 */
void Format::setFontUnderline(FontUnderline underline)
{
    setProperty(FormatPrivate::P_Font_Underline, underline);
}

/*!
 * Return whether the font is outline.
 */
bool Format::fontOutline() const
{
    return boolProperty(FormatPrivate::P_Font_Outline);
}

/*!
 * Turn on/off the outline font.
 */
void Format::setFontOutline(bool outline)
{
    setProperty(FormatPrivate::P_Font_Outline, outline);
}

/*!
 * Return the name of the font.
 */
QString Format::fontName() const
{
    return stringProperty(FormatPrivate::P_Font_Name);
}

/*!
 * Set the name of the font.
 */
void Format::setFontName(const QString &name)
{
    setProperty(FormatPrivate::P_Font_Name, name);
}

/*!
 * \internal
 */
bool Format::fontIndexValid() const
{
    return d->font_index_valid;
}

/*!
 * \internal
 */
int Format::fontIndex() const
{
    if (fontIndexValid())
        return d->font_index;

    return -1;
}

/*!
 * \internal
 */
void Format::setFontIndex(int index)
{
    d->font_index = index;
    d->font_index_valid = true;
}

/* Internal
 */
QByteArray Format::fontKey() const
{
    if (d->font_dirty) {
        QByteArray key;
        QDataStream stream(&key, QIODevice::WriteOnly);
        for (int i=FormatPrivate::P_Font_STARTID; i<FormatPrivate::P_Font_ENDID; ++i) {
            if (d->property.contains(i))
                stream << i << d->property[i];
        };

        const_cast<Format*>(this)->d->font_key = key;
        const_cast<Format*>(this)->d->font_dirty = false;
    }

    return d->font_key;
}

bool Format::hasFontData() const
{
    for (int i=FormatPrivate::P_Font_STARTID; i<FormatPrivate::P_Font_ENDID; ++i) {
        if (hasProperty(i))
            return true;
    }
    return false;
}

/*!
 * Return the horizontal alignment.
 */
Format::HorizontalAlignment Format::horizontalAlignment() const
{
    return static_cast<Format::HorizontalAlignment>(intProperty(FormatPrivate::P_Alignment_AlignH));
}

/*!
 * Set the horizontal alignment.
 */
void Format::setHorizontalAlignment(HorizontalAlignment align)
{
    if (hasProperty(FormatPrivate::P_Alignment_Indent)
            &&(align != AlignHGeneral && align != AlignLeft && align != AlignRight && align != AlignHDistributed)) {
        clearProperty(FormatPrivate::P_Alignment_Indent);
    }

    if (hasProperty(FormatPrivate::P_Alignment_ShinkToFit)
            && (align == AlignHFill || align == AlignHJustify || align == AlignHDistributed)) {
        clearProperty(FormatPrivate::P_Alignment_ShinkToFit);
    }

    setProperty(FormatPrivate::P_Alignment_AlignH, align);
}

/*!
 * Return the vertical alignment.
 */
Format::VerticalAlignment Format::verticalAlignment() const
{
    return static_cast<Format::VerticalAlignment>(intProperty(FormatPrivate::P_Alignment_AlignV));
}

/*!
 * Set the vertical alignment.
 */
void Format::setVerticalAlignment(VerticalAlignment align)
{
    setProperty(FormatPrivate::P_Alignment_AlignV, align);
}

/*!
 * Return whether the cell text is wrapped.
 */
bool Format::textWrap() const
{
    return boolProperty(FormatPrivate::P_Alignment_Wrap);
}

/*!
 * Enable the text wrap
 */
void Format::setTextWarp(bool wrap)
{
    if (wrap && hasProperty(FormatPrivate::P_Alignment_ShinkToFit))
        clearProperty(FormatPrivate::P_Alignment_ShinkToFit);

    setProperty(FormatPrivate::P_Alignment_Wrap, wrap);
}

/*!
 * Return the text rotation.
 */
int Format::rotation() const
{
    return intProperty(FormatPrivate::P_Alignment_Rotation);
}

/*!
 * Set the text roation. Must be in the range [0, 180] or 255.
 */
void Format::setRotation(int rotation)
{
    setProperty(FormatPrivate::P_Alignment_Rotation, rotation);
}

/*!
 * Return the text indentation level.
 */
int Format::indent() const
{
    return intProperty(FormatPrivate::P_Alignment_Indent);
}

/*!
 * Set the text indentation level. Must be less than or equal to 15.
 */
void Format::setIndent(int indent)
{
    if (indent && hasProperty(FormatPrivate::P_Alignment_AlignH)) {
        HorizontalAlignment hl = horizontalAlignment();

        if (hl != AlignHGeneral && hl != AlignLeft && hl!= AlignRight && hl!= AlignHJustify) {
            setHorizontalAlignment(AlignLeft);
        }
    }

    setProperty(FormatPrivate::P_Alignment_Indent, indent);
}

/*!
 * Return whether the cell is shrink to fit.
 */
bool Format::shrinkToFit() const
{
    return boolProperty(FormatPrivate::P_Alignment_ShinkToFit);
}

/*!
 * Turn on/off shrink to fit.
 */
void Format::setShrinkToFit(bool shink)
{
    if (shink && hasProperty(FormatPrivate::P_Alignment_Wrap))
        clearProperty(FormatPrivate::P_Alignment_Wrap);

    if (shink && hasProperty(FormatPrivate::P_Alignment_AlignH)) {
        HorizontalAlignment hl = horizontalAlignment();
        if (hl == AlignHFill || hl == AlignHJustify || hl == AlignHDistributed)
            setHorizontalAlignment(AlignLeft);
    }

    setProperty(FormatPrivate::P_Alignment_ShinkToFit, shink);
}

/*!
 * \internal
 */
bool Format::hasAlignmentData() const
{
    for (int i=FormatPrivate::P_Alignment_STARTID; i<FormatPrivate::P_Alignment_ENDID; ++i) {
        if (hasProperty(i))
            return true;
    }
    return false;
}

/*!
 * Set the border style.
 */
void Format::setBorderStyle(BorderStyle style)
{
    setLeftBorderStyle(style);
    setRightBorderStyle(style);
    setBottomBorderStyle(style);
    setTopBorderStyle(style);
}

/*!
 * Set the border color.
 */
void Format::setBorderColor(const QColor &color)
{
    setLeftBorderColor(color);
    setRightBorderColor(color);
    setTopBorderColor(color);
    setBottomBorderColor(color);
}

/*!
 * Return the left border style
 */
Format::BorderStyle Format::leftBorderStyle() const
{
    return static_cast<BorderStyle>(intProperty(FormatPrivate::P_Border_LeftStyle));
}

/*!
 * Set the left border style
 */
void Format::setLeftBorderStyle(BorderStyle style)
{
    setProperty(FormatPrivate::P_Border_LeftStyle, style);
}

/*!
 * Return the left border color
 */
QColor Format::leftBorderColor() const
{
    return colorProperty(FormatPrivate::P_Border_LeftColor);
}

void Format::setLeftBorderColor(const QColor &color)
{
    setProperty(FormatPrivate::P_Border_LeftColor, color);
}

Format::BorderStyle Format::rightBorderStyle() const
{
    return static_cast<BorderStyle>(intProperty(FormatPrivate::P_Border_RightStyle));
}

void Format::setRightBorderStyle(BorderStyle style)
{
    setProperty(FormatPrivate::P_Border_RightStyle, style);
}

QColor Format::rightBorderColor() const
{
    return colorProperty(FormatPrivate::P_Border_RightColor);
}

void Format::setRightBorderColor(const QColor &color)
{
    setProperty(FormatPrivate::P_Border_RightColor, color);
}

Format::BorderStyle Format::topBorderStyle() const
{
    return static_cast<BorderStyle>(intProperty(FormatPrivate::P_Border_TopStyle));
}

void Format::setTopBorderStyle(BorderStyle style)
{
    setProperty(FormatPrivate::P_Border_TopStyle, style);
}

QColor Format::topBorderColor() const
{
    return colorProperty(FormatPrivate::P_Border_TopColor);
}

void Format::setTopBorderColor(const QColor &color)
{
    setProperty(FormatPrivate::P_Border_TopColor, color);
}

Format::BorderStyle Format::bottomBorderStyle() const
{
    return static_cast<BorderStyle>(intProperty(FormatPrivate::P_Border_BottomStyle));
}

void Format::setBottomBorderStyle(BorderStyle style)
{
    setProperty(FormatPrivate::P_Border_BottomStyle, style);
}

QColor Format::bottomBorderColor() const
{
    return colorProperty(FormatPrivate::P_Border_BottomColor);
}

void Format::setBottomBorderColor(const QColor &color)
{
    setProperty(FormatPrivate::P_Border_BottomColor, color);
}

Format::BorderStyle Format::diagonalBorderStyle() const
{
    return static_cast<BorderStyle>(intProperty(FormatPrivate::P_Border_DiagonalStyle));
}

void Format::setDiagonalBorderStyle(BorderStyle style)
{
    setProperty(FormatPrivate::P_Border_DiagonalStyle, style);
}

Format::DiagonalBorderType Format::diagonalBorderType() const
{
    return static_cast<DiagonalBorderType>(intProperty(FormatPrivate::P_Border_DiagonalType));
}

void Format::setDiagonalBorderType(DiagonalBorderType style)
{
    setProperty(FormatPrivate::P_Border_DiagonalType, style);
}

QColor Format::diagonalBorderColor() const
{
    return colorProperty(FormatPrivate::P_Border_DiagonalColor);
}

void Format::setDiagonalBorderColor(const QColor &color)
{
    setProperty(FormatPrivate::P_Border_DiagonalColor, color);
}

bool Format::borderIndexValid() const
{
    return d->border_index_valid;
}

int Format::borderIndex() const
{
    return d->border_index;
}

void Format::setBorderIndex(int index)
{
    d->border_index = index;
}

/* Internal
 */
QByteArray Format::borderKey() const
{
    if (d->border_dirty) {
        QByteArray key;
        QDataStream stream(&key, QIODevice::WriteOnly);
        for (int i=FormatPrivate::P_Border_STARTID; i<FormatPrivate::P_Border_ENDID; ++i) {
            if (d->property.contains(i))
                stream << i << d->property[i];
        };

        const_cast<Format*>(this)->d->border_key = key;
        const_cast<Format*>(this)->d->border_dirty = false;
    }

    return d->border_key;
}

Format::FillPattern Format::fillPattern() const
{
    return static_cast<FillPattern>(intProperty(FormatPrivate::P_Fill_Pattern));
}

void Format::setFillPattern(FillPattern pattern)
{
    setProperty(FormatPrivate::P_Fill_Pattern, pattern);
}

QColor Format::patternForegroundColor() const
{
    return colorProperty(FormatPrivate::P_Fill_FgColor);
}

void Format::setPatternForegroundColor(const QColor &color)
{
    if (color.isValid() && !hasProperty(FormatPrivate::P_Fill_Pattern))
        setFillPattern(PatternSolid);
    setProperty(FormatPrivate::P_Fill_FgColor, color);
}

QColor Format::patternBackgroundColor() const
{
    return colorProperty(FormatPrivate::P_Fill_BgColor);
}

void Format::setPatternBackgroundColor(const QColor &color)
{
    if (color.isValid() && !hasProperty(FormatPrivate::P_Fill_Pattern))
        setFillPattern(PatternSolid);
    setProperty(FormatPrivate::P_Fill_BgColor, color);
}

bool Format::fillIndexValid() const
{
    return d->fill_index_valid;
}

int Format::fillIndex() const
{
    return d->fill_index;
}

void Format::setFillIndex(int index)
{
    d->fill_index = index;
}

/* Internal
 */
QByteArray Format::fillKey() const
{
    if (d->fill_dirty) {
        QByteArray key;
        QDataStream stream(&key, QIODevice::WriteOnly);
        for (int i=FormatPrivate::P_Fill_STARTID; i<FormatPrivate::P_Fill_ENDID; ++i) {
            if (d->property.contains(i))
                stream << i << d->property[i];
        };

        const_cast<Format*>(this)->d->fill_key = key;
        const_cast<Format*>(this)->d->fill_dirty = false;
    }

    return d->fill_key;
}

bool Format::hidden() const
{
    return boolProperty(FormatPrivate::P_Protection_Hidden);
}

void Format::setHidden(bool hidden)
{
    setProperty(FormatPrivate::P_Protection_Hidden, hidden);
}

bool Format::locked() const
{
    return boolProperty(FormatPrivate::P_Protection_Locked);
}

void Format::setLocked(bool locked)
{
    setProperty(FormatPrivate::P_Protection_Locked, locked);
}

/*!
    Returns true if the format is valid; otherwise returns false.
 */
bool Format::isValid() const
{
    return !d->property.isEmpty();
}

QByteArray Format::formatKey() const
{
    if (d->dirty) {
        QByteArray key;
        QDataStream stream(&key, QIODevice::WriteOnly);
        stream<<fontKey()<<borderKey()<<fillKey()<<numberFormatIndex();
        for (int i=FormatPrivate::P_OTHER_STARTID; i<FormatPrivate::P_OTHER_ENDID; ++i) {
            if (d->property.contains(i))
                stream<<i<<d->property[i];
        }
        d->formatKey = key;
        d->dirty = false;
    }

    return d->formatKey;
}

void Format::setXfIndex(int index)
{
    d->xf_index = index;
    d->xf_indexValid = true;
}

int Format::xfIndex() const
{
    return d->xf_index;
}

bool Format::xfIndexValid() const
{
    return !d->dirty && d->xf_indexValid;
}

void Format::setDxfIndex(int index)
{
    d->dxf_index = index;
    d->dxf_indexValid = true;
}

int Format::dxfIndex() const
{
    return d->dxf_index;
}

bool Format::dxfIndexValid() const
{
    return !d->dirty && d->dxf_indexValid;
}

bool Format::operator ==(const Format &format) const
{
    return this->formatKey() == format.formatKey();
}

bool Format::operator !=(const Format &format) const
{
    return this->formatKey() != format.formatKey();
}

bool Format::isDxfFormat() const
{
    return d->is_dxf_fomat;
}

int Format::theme() const
{
    return d->theme;
}

/*!
 * \internal
 */
QVariant Format::property(int propertyId) const
{
    if (d->property.contains(propertyId))
        return d->property[propertyId];
    return QVariant();
}

/*!
 * \internal
 */
void Format::setProperty(int propertyId, const QVariant &value)
{
    if (value.isValid()) {
        if (d->property.contains(propertyId) && d->property[propertyId] == value)
            return;
        d.detach();
        d->property[propertyId] = value;
    } else {
        if (!d->property.contains(propertyId))
            return;
        d.detach();
        d->property.remove(propertyId);
    }

    d->dirty = true;
    d->xf_indexValid = false;
    d->dxf_indexValid = false;

    if (propertyId >= FormatPrivate::P_Font_STARTID && propertyId < FormatPrivate::P_Font_ENDID) {
        d->font_dirty = true;
        d->font_index_valid = false;
    } else if (propertyId >= FormatPrivate::P_Border_STARTID && propertyId < FormatPrivate::P_Border_ENDID) {
        d->border_dirty = true;
        d->border_index_valid = false;
    } else if (propertyId >= FormatPrivate::P_Fill_STARTID && propertyId < FormatPrivate::P_Fill_ENDID) {
        d->fill_dirty = true;
        d->fill_index_valid = false;
    }
}

/*!
 * \internal
 */
void Format::clearProperty(int propertyId)
{
    setProperty(propertyId, QVariant());
}

/*!
 * \internal
 */
bool Format::hasProperty(int propertyId) const
{
    return d->property.contains(propertyId);
}

/*!
 * \internal
 */
bool Format::boolProperty(int propertyId) const
{
    if (!hasProperty(propertyId))
        return false;

    const QVariant prop = d->property[propertyId];
    if (prop.userType() != QMetaType::Bool)
        return false;
    return prop.toBool();
}

/*!
 * \internal
 */
int Format::intProperty(int propertyId) const
{
    if (!hasProperty(propertyId))
        return 0;

    const QVariant prop = d->property[propertyId];
    if (prop.userType() != QMetaType::Int)
        return 0;
    return prop.toInt();
}

/*!
 * \internal
 */
double Format::doubleProperty(int propertyId) const
{
    if (!hasProperty(propertyId))
        return 0;

    const QVariant prop = d->property[propertyId];
    if (prop.userType() != QMetaType::Double && prop.userType() != QMetaType::Float)
        return 0;
    return prop.toDouble();
}

/*!
 * \internal
 */
QString Format::stringProperty(int propertyId) const
{
    if (!hasProperty(propertyId))
        return QString();

    const QVariant prop = d->property[propertyId];
    if (prop.userType() != QMetaType::QString)
        return QString();
    return prop.toString();
}

/*!
 * \internal
 */
QColor Format::colorProperty(int propertyId) const
{
    if (!hasProperty(propertyId))
        return QColor();

    const QVariant prop = d->property[propertyId];
    if (prop.userType() != qMetaTypeId<QColor>())
        return QColor();
    return qvariant_cast<QColor>(prop);
}

QT_END_NAMESPACE_XLSX

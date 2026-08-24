#pragma once

#include <QString>
#include <QtGlobal>

namespace OpenWordPad {

enum class UnitType {
    Inches,
    Centimeters,
    Points,
    Picas
};

class Units {
public:
    static constexpr double DPI = 96.0;
    static constexpr double POINTS_PER_INCH = 72.0;
    static constexpr double MM_PER_INCH = 25.4;
    static constexpr double PICAS_PER_INCH = 6.0;
    static constexpr double TWIPS_PER_INCH = 1440.0;
    static constexpr double TWIPS_PER_POINT = 20.0;

    // Convert from unit to pixels (at screen DPI)
    static double toPixels(double value, UnitType unit, double dpi = DPI) {
        switch (unit) {
            case UnitType::Inches:
                return value * dpi;
            case UnitType::Centimeters:
                return (value / 2.54) * dpi;
            case UnitType::Points:
                return (value / POINTS_PER_INCH) * dpi;
            case UnitType::Picas:
                return (value / PICAS_PER_INCH) * dpi;
        }
        return value;
    }

    // Convert from pixels to unit
    static double fromPixels(double pixels, UnitType unit, double dpi = DPI) {
        switch (unit) {
            case UnitType::Inches:
                return pixels / dpi;
            case UnitType::Centimeters:
                return (pixels / dpi) * 2.54;
            case UnitType::Points:
                return (pixels / dpi) * POINTS_PER_INCH;
            case UnitType::Picas:
                return (pixels / dpi) * PICAS_PER_INCH;
        }
        return pixels;
    }

    // Convert points to/from twips (used extensively in RTF)
    static int pointsToTwips(double points) {
        return static_cast<int>(points * TWIPS_PER_POINT + 0.5);
    }

    static double twipsToPoints(int twips) {
        return static_cast<double>(twips) / TWIPS_PER_POINT;
    }

    static int inchesToTwips(double inches) {
        return static_cast<int>(inches * TWIPS_PER_INCH + 0.5);
    }

    static double twipsToInches(int twips) {
        return static_cast<double>(twips) / TWIPS_PER_INCH;
    }

    static QString unitSuffix(UnitType unit) {
        switch (unit) {
            case UnitType::Inches: return QStringLiteral("in");
            case UnitType::Centimeters: return QStringLiteral("cm");
            case UnitType::Points: return QStringLiteral("pt");
            case UnitType::Picas: return QStringLiteral("pi");
        }
        return QStringLiteral("");
    }

    static QString unitName(UnitType unit) {
        switch (unit) {
            case UnitType::Inches: return QStringLiteral("Inches");
            case UnitType::Centimeters: return QStringLiteral("Centimeters");
            case UnitType::Points: return QStringLiteral("Points");
            case UnitType::Picas: return QStringLiteral("Picas");
        }
        return QStringLiteral("");
    }
};

} // namespace OpenWordPad

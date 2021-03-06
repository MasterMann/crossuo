﻿// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "BaseGUI.h"

class CGUISlider : public CBaseGUI
{
public:
    //!ИД картинки в выбранном состоянии
    uint16_t GraphicSelected = 0;

    //!ИД картинки в зажатом состоянии
    uint16_t GraphicPressed = 0;

    //!ИД картинки фона
    uint16_t BackgroundGraphic = 0;

    //!Сборный фон
    bool CompositeBackground = false;

    //!Вертикальный или горизонтальный
    bool Vertical = false;

    //!Длина
    int Length = 0;

    //!Смещение в процентах
    float Percents = 0.0f;

    //!Смещение
    int Offset = 0;

    //!Минимальное значение
    int MinValue = 0;

    //!Максимальное значение
    int MaxValue = 0;

    //!Текущее значение
    int Value = 0;

    //!Имеет текст
    bool HaveText = false;

    //!Позиция текста
    SLIDER_TEXT_POSITION TextPosition = STP_RIGHT;

    //!Шрифт текста
    uint8_t Font = 0;

    //!Цвет текста
    uint16_t TextColor = 0;

    //!Текст в юникоде
    bool Unicode = true;

    //!Ширина текста
    int TextWidth = 0;

    //!Ориентация текста
    TEXT_ALIGN_TYPE Align = TS_LEFT;

    //!Флаги текста
    uint16_t TextFlags = 0;

    //!Координата текста по оси X
    int TextX = 0;

    //!Координата текста по оси Y
    int TextY = 0;

    //!Шаг скроллера
    int ScrollStep = 15;

    //!Время последнего скроллинга
    uint32_t LastScrollTime = 0;

    //!Стандартное смещение текста
    int DefaultTextOffset = 2;

private:
    //!Текстура текста
    CGLTextTexture Text{ CGLTextTexture() };

public:
    CGUISlider(
        int serial,
        uint16_t graphic,
        uint16_t graphicSelected,
        uint16_t graphicPressed,
        uint16_t backgroundGraphic,
        bool compositeBackground,
        bool vertical,
        int x,
        int y,
        int length,
        int minValue,
        int maxValue,
        int value);
    virtual ~CGUISlider();

    virtual bool IsPressedOuthit() { return true; }

    virtual CSize GetSize();

    //!Скроллинг
    virtual void OnScroll(bool up, int delay);

    //!Нажатие на компоненту
    virtual void OnClick(int x, int y);

    //!Обновить текст
    void UpdateText();

    //!Пересчитать смещения
    virtual void CalculateOffset();

    //!Установить параметры текста
    void SetTextParameters(
        bool haveText,
        SLIDER_TEXT_POSITION textPosition,
        uint8_t font,
        uint16_t color,
        bool unicode,
        int textWidth = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        uint16_t textFlags = 0);

    virtual void PrepareTextures();

    virtual uint16_t GetDrawGraphic();

    virtual void Draw(bool checktrans = false);
    virtual bool Select();

    virtual void OnMouseEnter();
    virtual void OnMouseExit();
};

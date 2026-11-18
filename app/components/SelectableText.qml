import QtQuick

// Текст, который можно выделить мышью и скопировать (Ctrl+C).
// Работает как Text: размер, цвет и жирность задаются так же, но обрезается
// по ширине без многоточия (TextEdit не умеет elide).
TextEdit {
    readOnly: true
    selectByMouse: true
    wrapMode: TextEdit.NoWrap
    clip: true
    color: Theme.text
    selectionColor: Theme.highlight
    selectedTextColor: Theme.highlightedText

    // выделение снимается, когда фокус уходит на другое поле
    onActiveFocusChanged: if (!activeFocus) deselect()
}

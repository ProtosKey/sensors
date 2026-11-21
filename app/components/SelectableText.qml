import QtQuick

TextEdit {
    id: selectableText

    readOnly: true
    selectByMouse: true
    wrapMode: TextEdit.NoWrap
    clip: true
    color: Theme.text
    selectionColor: Theme.highlight
    selectedTextColor: Theme.highlightedText

    onActiveFocusChanged: if (!selectableText.activeFocus) selectableText.deselect()
}

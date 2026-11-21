#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "config.h"
AppConfig g_Config = {};
#include "arabic_reshaper.h"

static size_t CountChar(const std::wstring& text, wchar_t ch) {
    size_t count = 0;
    for (wchar_t value : text) {
        if (value == ch) ++count;
    }
    return count;
}

int main() {
    std::wstring s = L"محمد طارق رجب منصور حسن الورد";
    auto res = ArabicBiDi::ProcessArabicWithMap(s);
    for (size_t i = 0; i < res.visual.size(); ++i) {
        printf("0x%04X ", (unsigned)res.visual[i]);
    }
    printf("\n");

    const std::wstring repeatedLetters = L"سس";
    const auto repeatedResult = ArabicBiDi::ProcessArabicWithMap(repeatedLetters);
    assert(repeatedResult.visual.size() == repeatedLetters.size());
    assert(repeatedResult.v2l.size() == repeatedLetters.size());

    const std::wstring bracketText = L"((سس))";
    const auto bracketResult = ArabicBiDi::ProcessArabicWithMap(bracketText);
    assert(CountChar(bracketResult.visual, L'(') == CountChar(bracketText, L'('));
    assert(CountChar(bracketResult.visual, L')') == CountChar(bracketText, L')'));

    g_Config.MirrorBrackets = true;
    const std::wstring editableWord = L"طارق";
    const auto wordMap = ArabicBiDi::ProcessArabicWithMap(editableWord);
    size_t alifVisualIndex = wordMap.v2l.size();
    for (size_t i = 0; i < wordMap.v2l.size(); ++i) {
        if (wordMap.v2l[i] == 1) {
            alifVisualIndex = i;
            break;
        }
    }
    assert(alifVisualIndex < wordMap.v2l.size());

    std::wstring editedWord = editableWord;
    editedWord.erase(wordMap.v2l[alifVisualIndex], 1);
    const auto editedMap = ArabicBiDi::ProcessArabicWithMap(editedWord);
    const size_t insertionPosition =
        ArabicBiDi::VisualCaretToLogicalInsertion(
            editedWord, editedMap, alifVisualIndex);
    editedWord.insert(insertionPosition, 1, L'ن');
    assert(editedWord == L"طنرق");
    assert(editedWord.size() == editableWord.size());

    const std::wstring parenthesizedText = L"(طارق)";
    const auto parenthesizedResult =
        ArabicBiDi::ProcessArabicWithMap(parenthesizedText);
    assert(CountChar(parenthesizedResult.visual, L'(') == 1);
    assert(CountChar(parenthesizedResult.visual, L')') == 1);
    assert(parenthesizedResult.visual.size() == parenthesizedText.size());
    assert(parenthesizedResult.visual.front() == L'(');
    assert(parenthesizedResult.visual.back() == L')');
    const wchar_t openingParen = L'(';
    assert(ArabicBiDi::MirrorBracket(openingParen) == L')');

    AppConfig limits = {};
    limits.MaxUsernameLength = 25;
    limits.MaxChatLineLength = 45;
    limits.openAllchat = true;
    assert(GetInputCharacterLimit(true, limits) == 25);
    assert(GetInputCharacterLimit(false, limits) == 45);
    assert(CanFitInputCharacters(24, 1, GetInputCharacterLimit(true, limits)));
    assert(!CanFitInputCharacters(25, 1, GetInputCharacterLimit(true, limits)));
    assert(CanFitInputCharacters(44, 1, GetInputCharacterLimit(false, limits)));
    assert(!CanFitInputCharacters(45, 1, GetInputCharacterLimit(false, limits)));
    assert(CanFitInputCharacters(20, 5, GetInputCharacterLimit(true, limits)));
    assert(!CanFitInputCharacters(21, 5, GetInputCharacterLimit(true, limits)));
    assert(!IsChatEditBoxFlag(0, true));
    assert(IsChatEditBoxFlag(1, true));
    assert(IsChatEditBoxFlag(0, false));
    assert(!IsChatEditBoxFlag(1, false));
    assert(!IsLoginInputAfterEnter(true, 0));
    assert(IsLoginInputAfterEnter(true, 1));
    assert(IsLoginInputAfterEnter(false, 0));
    assert(IsLoginInputAfterEnter(false, 1));
    const bool p520ChatBox = IsChatEditBoxFlag(1, true);
    const bool p520LoginBox = !IsChatEditBoxFlag(0, true);
    assert(p520LoginBox);
    assert(GetInputCharacterLimit(p520LoginBox, limits) == 25);
    const bool p520InternalChatBox = IsChatEditBoxFlag(1, true);
    assert(p520InternalChatBox);
    assert(GetInputCharacterLimit(!p520InternalChatBox, limits) == 45);
    limits.MaxUsernameLength = 40;
    limits.MaxChatLineLength = 100;
    assert(GetInputCharacterLimit(true, limits) == 25);
    assert(GetInputCharacterLimit(false, limits) == 45);
    limits.MaxUsernameLength = 20;
    limits.MaxChatLineLength = 35;
    assert(GetInputCharacterLimit(true, limits) == 20);
    assert(GetInputCharacterLimit(false, limits) == 35);
    return 0;
}

/**
 * C++ wrapper functions for raylib.
 */
#ifndef RAYLIB_CPP_INCLUDE_FUNCTIONS_HPP_
#define RAYLIB_CPP_INCLUDE_FUNCTIONS_HPP_

#include <string>
#include <vector>

#include "./raylib.hpp"
#include "./RadiansDegrees.hpp"

/**
 * Allow changing the declare type for all raylib-cpp global functions. Defaults to static.
 */
#ifndef RLCPPAPI
#define RLCPPAPI static
#endif

namespace raylib {

/**
 * Initialize window and OpenGL context
 */
RLCPPAPI inline void InitWindow(int width, int height, const std::string_view title = "raylib") {
    ::InitWindow(width, height, title.data());
}

/**
 * Set title for window
 */
RLCPPAPI inline void SetWindowTitle(const std::string_view title) {
    ::SetWindowTitle(title.data());
}

/**
 * Get the human-readable, UTF-8 encoded name of the primary monitor
 */
RLCPPAPI inline std::string GetMonitorName(int monitor = 0) {
    return ::GetMonitorName(monitor);
}

/**
 * Set clipboard text content
 */
RLCPPAPI inline void SetClipboardText(const std::string_view text) {
    ::SetClipboardText(text.data());
}

/**
 * Get clipboard text content
 */
RLCPPAPI inline std::string GetClipboardText() {
    return ::GetClipboardText();
}

/**
 * Takes a screenshot of current screen (saved a .png)
 */
RLCPPAPI inline void TakeScreenshot(const std::string_view fileName) {
    ::TakeScreenshot(fileName.data());
}

/**
 * Get gamepad internal name id
 */
RLCPPAPI inline std::string GetGamepadName(int gamepad) {
    return ::GetGamepadName(gamepad);
}

/**
 * Load text data from file (read)
 */
RLCPPAPI std::string LoadFileText(const std::string_view fileName) {
    char* text = ::LoadFileText(fileName.data());
    std::string output(text);
    ::UnloadFileText(text);
    return output;
}

/**
 * Save text data to file (write)
 */
RLCPPAPI inline bool SaveFileText(const std::string_view fileName, const std::string_view text) {
    return ::SaveFileText(fileName.data(), const_cast<char*>(text.data()));
}

/**
 * Check if file exists
 */
RLCPPAPI inline bool FileExists(const std::string_view fileName) {
    return ::FileExists(fileName.data());
}

/**
 * Check if directory path exists
 */
RLCPPAPI inline bool DirectoryExists(const std::string_view dirPath) {
    return ::DirectoryExists(dirPath.data());
}

/**
 * Check file extension (including point: .png, .wav)
 */
RLCPPAPI inline bool IsFileExtension(const std::string_view fileName, const std::string_view ext) {
    return ::IsFileExtension(fileName.data(), ext.data());
}

/**
 * Get pointer to extension for a filename string (including point: ".png")
 */
RLCPPAPI inline std::string GetFileExtension(const std::string_view fileName) {
    return ::GetFileExtension(fileName.data());
}

/**
 * Get pointer to filename for a path string
 */
RLCPPAPI inline std::string GetFileName(const std::string_view filePath) {
    return ::GetFileName(filePath.data());
}

/**
 * Get filename string without extension
 */
RLCPPAPI inline std::string GetFileNameWithoutExt(const std::string_view filePath) {
    return ::GetFileNameWithoutExt(filePath.data());
}

/**
 * Get full path for a given fileName with path
 */
RLCPPAPI inline std::string GetDirectoryPath(const std::string_view filePath) {
    return ::GetDirectoryPath(filePath.data());
}

/**
 * Get previous directory path for a given path
 */
RLCPPAPI inline std::string GetPrevDirectoryPath(const std::string_view dirPath) {
    return ::GetPrevDirectoryPath(dirPath.data());
}

/**
 * Get current working directory
 */
RLCPPAPI inline std::string GetWorkingDirectory() {
    return ::GetWorkingDirectory();
}

/**
 * Get filenames in a directory path
 */
RLCPPAPI std::vector<std::string> LoadDirectoryFiles(const std::string_view dirPath) {
    FilePathList files = ::LoadDirectoryFiles(dirPath.data());
    std::vector<std::string> output(files.paths, files.paths + files.count);
    ::UnloadDirectoryFiles(files);
    return output;
}

/**
 * Change working directory, return true on success
 */
RLCPPAPI inline bool ChangeDirectory(const std::string_view dir) {
    return ::ChangeDirectory(dir.data());
}

/**
 * Get dropped files names
 */
RLCPPAPI std::vector<std::string> LoadDroppedFiles() {
    if (!::IsFileDropped()) {
        return std::vector<std::string>();
    }
    FilePathList files = ::LoadDroppedFiles();
    std::vector<std::string> output(files.paths, files.paths + files.count);
    ::UnloadDroppedFiles(files);
    return output;
}

/**
 * Get file modification time (last write time)
 */
RLCPPAPI inline long GetFileModTime(const std::string_view fileName) { // NOLINT
    return ::GetFileModTime(fileName.data());
}

/**
 * Open URL with default system browser (if available)
 */
RLCPPAPI inline void OpenURL(const std::string_view url) {
    return ::OpenURL(url.data());
}

/**
 * Load an image.
 */
RLCPPAPI inline ::Image LoadImage(const std::string_view fileName) {
    return ::LoadImage(fileName.data());
}

/**
 * Load an image from RAW file data
 */
RLCPPAPI inline ::Image LoadImageRaw(const std::string_view fileName,
        int width, int height,
        int format, int headerSize) {
    return ::LoadImageRaw(fileName.data(), width, height, format, headerSize);
}

/**
 * Load animated image data
 */
RLCPPAPI inline ::Image LoadImageAnim(const std::string_view fileName, int *frames) {
    return ::LoadImageAnim(fileName.data(), frames);
}

/**
 * Load animated image data
 */
RLCPPAPI inline ::Image LoadImageAnim(const std::string_view fileName, std::span<int> frames) {
    return ::LoadImageAnim(fileName.data(), frames.data());
}

/**
 * Load image from memory buffer, fileType refers to extension like "png"
 */
RLCPPAPI inline ::Image LoadImageFromMemory(const std::string_view fileType,
        const unsigned char *fileData,
        int dataSize) {
    return ::LoadImageFromMemory(fileType.data(), fileData, dataSize);
}

/**
 * Export image data to file
 */
RLCPPAPI inline bool ExportImage(const Image& image, const std::string_view fileName) {
    return ::ExportImage(image, fileName.data());
}

/**
 * Export image as code file (.h) defining an array of bytes
 */
RLCPPAPI inline bool ExportImageAsCode(const Image& image, const std::string_view fileName) {
    return ::ExportImageAsCode(image, fileName.data());
}

/**
 * Draw text (using default font)
 */
RLCPPAPI inline void DrawText(const std::string_view text, int posX, int posY, int fontSize, ::Color color) {
    ::DrawText(text.data(), posX, posY, fontSize, color);
}

/**
 * Draw text using font and additional parameters
 */
RLCPPAPI inline void DrawTextEx(const Font& font, const std::string_view text, Vector2 position,
        float fontSize, float spacing, ::Color tint) {
    ::DrawTextEx(font, text.data(), position, fontSize, spacing, tint);
}

/**
 * Draw text using Font and pro parameters (rotation)
 */
RLCPPAPI inline void DrawTextPro(const Font& font, const std::string_view text, Vector2 position,
        Vector2 origin, Radian rotation, float fontSize, float spacing, ::Color tint) {
    ::DrawTextPro(font, text.data(), position, origin, rotation, fontSize, spacing, tint);
}

/**
 * Load font from file (filename must include file extension)
 */
RLCPPAPI inline ::RLFont LoadFont(const std::string_view fileName) {
    return ::LoadFont(fileName.data());
}

/**
 * Load font from file (filename must include file extension)
 */
RLCPPAPI inline ::RLFont LoadFontEx(const std::string_view fileName, int fontSize, int *fontChars, int charsCount) {
    return ::LoadFontEx(fileName.data(), fontSize, fontChars, charsCount);
}

/**
 * Load font from file (filename must include file extension)
 */
RLCPPAPI inline ::RLFont LoadFontEx(const std::string_view fileName, int fontSize, std::span<int> fontChars) {
    return ::LoadFontEx(fileName.data(), fontSize, fontChars.data(), fontChars.size());
}

/**
 * Measure string width for default font
 */
RLCPPAPI inline int MeasureText(const std::string_view text, int fontSize) {
    return ::MeasureText(text.data(), fontSize);
}

/**
 * Check if two text string are equal
 */
RLCPPAPI inline bool TextIsEqual(const std::string_view text1, const std::string_view text2) {
    return ::TextIsEqual(text1.data(), text2.data());
}

/**
 * Check if two text string are equal
 */
RLCPPAPI inline unsigned int TextLength(const std::string_view text) {
    return ::TextLength(text.data());
}

/**
 * Get text length, checks for '\0' ending
 */
RLAPI inline std::string TextSubtext(const std::string_view text, int position, int length) {
    return ::TextSubtext(text.data(), position, length);
}

/**
 * Replace text string
 */
RLAPI inline std::string TextReplace(const std::string_view text, const std::string_view replace, const std::string_view by) {
    const char* input = text.data();
    char* output = ::TextReplace(const_cast<char*>(input), replace.data(), by.data());
    if (output != NULL) {
        std::string stringOutput(output);
        free(output);
        return stringOutput;
    }
    return "";
}

/**
 * Insert text in a position
 */
RLAPI inline std::string TextInsert(const std::string_view text, const std::string_view insert, int position) {
    char* output = ::TextInsert(text.data(), insert.data(), position);
    if (output != NULL) {
        std::string stringOutput(output);
        free(output);
        return stringOutput;
    }
    return "";
}

/**
 * Split text into multiple strings
 */
RLAPI inline std::vector<std::string> TextSplit(const std::string_view text, char delimiter) {
    int count;
    const char** split = ::TextSplit(text.data(), delimiter, &count);
    return std::vector<std::string>(split, split + count);
}

/**
 * Find first text occurrence within a string
 */
RLAPI inline int TextFindIndex(const std::string_view text, const std::string_view find) {
    return ::TextFindIndex(text.data(), find.data());
}

/**
 * Get upper case version of provided string
 */
RLAPI inline std::string TextToUpper(const std::string_view text) {
    return ::TextToUpper(text.data());
}

/**
 * Get lower case version of provided string
 */
RLAPI inline std::string TextToLower(const std::string_view text) {
    return ::TextToLower(text.data());
}

/**
 * Get Pascal case notation version of provided string
 */
RLAPI inline std::string TextToPascal(const std::string_view text) {
    return ::TextToPascal(text.data());
}

/**
 * Get integer value from text (negative values not supported)
 */
RLAPI inline int TextToInteger(const std::string_view text) {
    return ::TextToInteger(text.data());
}

/**
 * Ensures that the given string_view points to null terminated memory
 * NOTE: The view will be invalidated by subsequent calls to the function...
*/
RLAPI inline std::string_view NullTerminated(const std::string_view s) {
    static std::string memory;
    memory = s;
    return {memory.c_str(), memory.size() + 1};
}

}  // namespace raylib

#endif  // RAYLIB_CPP_INCLUDE_FUNCTIONS_HPP_

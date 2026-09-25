/**
 * @file util/json.h
 * @brief JSON 工具类，提供 JSON 的解析与生成功能。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 */

/*
Copyright (c) 2026 MrXie1109

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <cmath>
#include <cstdio>
#include <cstring>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "../core/csexc.h"

namespace console {
    /// @brief JSON 值类，用于表示 JSON 中的任意值 (null、布尔、数字、字符串、数组或对象)。
    class JsonValue;
    /// @brief JSON 数组类型，由 JsonValue 组成的动态数组。
    using JsonArray = std::vector<JsonValue>;
    /// @brief JSON 对象类型，由字符串到 JsonValue 的有序映射。
    using JsonObject = std::map<std::string, JsonValue>;

    class JsonValue {
    public:
        /// @brief JSON 值类型枚举，表示 JSON 中可能出现的六种数据类型。
        enum class Type { Null, Bool, Number, String, Array, Object };

        /// @brief 默认构造函数，创建一个 null 类型的 JSON 值。
        JsonValue() : type_(Type::Null) {}

        /// @brief 由 nullptr 构造，创建一个 null 类型的 JSON 值。
        JsonValue(std::nullptr_t) : type_(Type::Null) {}

        /// @brief 由布尔值构造 JSON 值。
        JsonValue(bool b) : type_(Type::Bool), bool_(b) {}

        /// @brief 由双精度浮点数构造 JSON 值。
        JsonValue(double n) : type_(Type::Number), num_(n) {}

        /// @brief 由整数构造 JSON 值。
        JsonValue(int n) : type_(Type::Number), num_(static_cast<double>(n)) {}

        /// @brief 由字符串构造 JSON 值。
        JsonValue(std::string s) :
            type_(Type::String), str_(new std::string(std::move(s))) {}

        /// @brief 由 C 风格字符串构造 JSON 值。
        JsonValue(const char *s) :
            type_(Type::String), str_(new std::string(s)) {}

        /// @brief 由 JSON 数组构造 JSON 值。
        JsonValue(JsonArray a) :
            type_(Type::Array), arr_(new JsonArray(std::move(a))) {}

        /// @brief 由 JSON 对象构造 JSON 值。
        JsonValue(JsonObject o) :
            type_(Type::Object), obj_(new JsonObject(std::move(o))) {}

        /// @brief 拷贝构造函数。
        JsonValue(const JsonValue &other) : type_(other.type_) {
            switch (type_) {
            case Type::Null:
                break;
            case Type::Bool:
                bool_ = other.bool_;
                break;
            case Type::Number:
                num_ = other.num_;
                break;
            case Type::String:
                str_ = new std::string(*other.str_);
                break;
            case Type::Array:
                arr_ = new JsonArray(*other.arr_);
                break;
            case Type::Object:
                obj_ = new JsonObject(*other.obj_);
                break;
            }
        }

        /// @brief 移动构造函数。
        JsonValue(JsonValue &&other) noexcept : type_(other.type_) {
            switch (type_) {
            case Type::Null:
                break;
            case Type::Bool:
                bool_ = other.bool_;
                break;
            case Type::Number:
                num_ = other.num_;
                break;
            case Type::String:
                str_       = other.str_;
                other.str_ = nullptr;
                break;
            case Type::Array:
                arr_       = other.arr_;
                other.arr_ = nullptr;
                break;
            case Type::Object:
                obj_       = other.obj_;
                other.obj_ = nullptr;
                break;
            }
            other.type_ = Type::Null;
        }

        /// @brief 拷贝赋值与移动赋值 (copy-and-swap)。
        JsonValue &operator=(JsonValue other) noexcept {
            swap(other);
            return *this;
        }

        /// @brief 析构函数，释放动态分配的字符串 / 数组 / 对象。
        ~JsonValue() {
            switch (type_) {
            case Type::String:
                delete str_;
                break;
            case Type::Array:
                delete arr_;
                break;
            case Type::Object:
                delete obj_;
                break;
            default:
                break;
            }
        }

        /**
         * @brief 交换两个 JSON 值。
         * @note 这看起来很诡异，但确实应该这么做。
         *       因为它是一个 trivially relocatable。
         *       有更好的办法，但太麻烦，不如就这样了。
         */
        void swap(JsonValue &other) noexcept {
            if (this == &other) return;
            char tmp[sizeof(JsonValue)];
            memcpy(tmp, this, sizeof(JsonValue));
            memcpy(static_cast<void *>(this), &other, sizeof(JsonValue));
            memcpy(static_cast<void *>(&other), tmp, sizeof(JsonValue));
        }

        /// @brief 获取 JSON 值的类型。
        Type type() const { return type_; }

        /// @brief 判断 JSON 值是否为 null 类型。
        bool is_null() const { return type_ == Type::Null; }
        /// @brief 判断 JSON 值是否为布尔类型。
        bool is_bool() const { return type_ == Type::Bool; }
        /// @brief 判断 JSON 值是否为数字类型。
        bool is_number() const { return type_ == Type::Number; }
        /// @brief 判断 JSON 值是否为字符串类型。
        bool is_string() const { return type_ == Type::String; }
        /// @brief 判断 JSON 值是否为数组类型。
        bool is_array() const { return type_ == Type::Array; }
        /// @brief 判断 JSON 值是否为对象类型。
        bool is_object() const { return type_ == Type::Object; }

        /**
         * @brief 将 JSON 值作为布尔值返回。
         * @return 布尔值。
         * @throw TypeError 若当前值不是布尔类型。
         */
        bool boolean() const {
            if (!is_bool()) throw TypeError("not a bool");
            return bool_;
        }

        /**
         * @brief 隐式转换为布尔值，等价于调用 boolean()。
         * @note 这不是 truthiness 检查接口，
         *       而是与其他隐式转换相提并论的存在，
         *       绝不能把他声明为 explicit！！！
         */
        operator bool() const { return boolean(); }

        /**
         * @brief 将 JSON 值作为数字返回。
         * @return 双精度浮点数。
         * @throw TypeError 若当前值不是数字类型。
         */
        double number() const {
            if (!is_number()) throw TypeError("not a number");
            return num_;
        }

        /**
         * @brief 隐式转换为算术类型 (不含 bool 与各类字符类型)。
         * @tparam T 目标算术类型。
         * @return 转换后的算术值。
         * @throw TypeError 若当前值不是数字类型。
         */
        template <class T,
            class = typename std::enable_if<
                std::is_arithmetic<typename std::decay<T>::type>::value
                && !std::is_same<typename std::decay<T>::type, bool>::value
                && !std::is_same<typename std::decay<T>::type, char>::value
                && !std::is_same<typename std::decay<T>::type,
                    signed char>::value
                && !std::is_same<typename std::decay<T>::type,
                    unsigned char>::value
                && !std::is_same<typename std::decay<T>::type, wchar_t>::value
                && !std::is_same<typename std::decay<T>::type, char16_t>::value
                && !std::is_same<typename std::decay<T>::type,
                    char32_t>::value>::type>
        operator T() const {
            return number();
        }

        /**
         * @brief 将 JSON 值作为字符串返回。
         * @return 字符串的常量引用。
         * @throw TypeError 若当前值不是字符串类型。
         */
        const std::string &string() const {
            if (!is_string()) throw TypeError("not a string");
            return *str_;
        }

        /// @brief 隐式转换为字符串，等价于调用 string()。
        operator std::string() const { return string(); }

        /**
         * @brief 将 JSON 值作为数组返回。
         * @return JsonArray 的常量引用。
         * @throw TypeError 若当前值不是数组类型。
         */
        const JsonArray &array() const {
            if (!is_array()) throw TypeError("not an array");
            return *arr_;
        }

        /// @brief 隐式转换为 JSON 数组，等价于调用 array()。
        operator JsonArray() const { return array(); }

        /**
         * @brief 将 JSON 值作为对象返回。
         * @return JsonObject 的常量引用。
         * @throw TypeError 若当前值不是对象类型。
         */
        const JsonObject &object() const {
            if (!is_object()) throw TypeError("not an object");
            return *obj_;
        }

        /// @brief 隐式转换为 JSON 对象，等价于调用 object()。
        operator JsonObject() const { return object(); }

        /**
         * @brief 获取 JSON 值的大小。
         * @return 若当前值为数组则返回元素个数，若为对象则返回键值对个数。
         * @throw TypeError 若当前值既不是数组也不是对象。
         */
        size_t size() const {
            if (is_array()) return arr_->size();
            if (is_object()) return obj_->size();
            throw TypeError("not an array or object");
        }

        /**
         * @brief 以数组下标访问 JSON 值。
         * @param i 数组下标。
         * @return 对应位置 JSON 值的引用。
         * @throw TypeError 若当前值不是数组类型。
         * @throw IndexError 下标越界时抛出。
         */
        JsonValue &operator[](size_t i) {
            if (!is_array()) throw TypeError("not an array");
            if (i >= arr_->size()) throw IndexError("index out of range");
            return (*arr_)[i];
        }

        /**
         * @brief 以数组下标访问 JSON 值 (常量版本)。
         * @param i 数组下标。
         * @return 对应位置 JSON 值的常量引用。
         * @throw TypeError 若当前值不是数组类型。
         * @throw IndexError 下标越界时抛出。
         */
        const JsonValue &operator[](size_t i) const {
            if (!is_array()) throw TypeError("not an array");
            if (i >= arr_->size()) throw IndexError("index out of range");
            return (*arr_)[i];
        }

        /**
         * @brief 以对象键访问 JSON 值。
         * @param key 对象键。
         * @return 对应键 JSON 值的引用。
         * @note 键不存在时创建。
         * @throw TypeError 若当前值不是对象类型。
         */
        JsonValue &operator[](const std::string &key) {
            if (!is_object()) throw TypeError("not an object");
            return (*obj_)[key];
        }

        /**
         * @brief 以对象键访问 JSON 值 (常量版本)。
         * @param key 对象键。
         * @return 对应键 JSON 值的常量引用。
         * @throw TypeError 若当前值不是对象类型。
         * @throw IndexError 键不存在时抛出。
         */
        const JsonValue &operator[](const std::string &key) const {
            if (!is_object()) throw TypeError("not an object");
            auto it = obj_->find(key);
            if (it == obj_->end()) throw IndexError("key not found");
            return it->second;
        }

        /**
         * @brief 以字符串字面量作为对象键访问 JSON 值。
         * @tparam N 字符串字面量长度 (含结尾 '\0')。
         * @param key 对象键，以字符串字面量形式给出。
         * @return 对应键 JSON 值的引用。
         * @note 键不存在时创建。
         * @throw TypeError 若当前值不是对象类型。
         */
        template <size_t N>
        JsonValue &operator[](const char (&key)[N]) {
            return (*this)[std::string(key)];
        }

        /**
         * @brief 以 C 风格字符串作为对象键访问 JSON 值。
         * @param key 对象键，以 C 风格字符串形式给出。
         * @return 对应键 JSON 值的常量引用。
         * @throw TypeError 若当前值不是对象类型。
         * @throw IndexError 键不存在时抛出。
         */
        const JsonValue &operator[](const char *key) const {
            return (*this)[std::string(key)];
        }

        /**
         * @brief 将 JSON 值序列化为字符串。
         * @return 序列化后的 JSON 字符串。
         */
        std::string dump() const {
            std::ostringstream os;
            serialize(os);
            return os.str();
        }

        /**
         * @brief 输出流运算符重载，将 JSON 值序列化后写入输出流。
         * @tparam CharT 输出流的字符类型。
         * @tparam Traits 输出流的字符特性类型。
         * @param os 目标输出流。
         * @param jv 待输出的 JSON 值。
         * @return 输出流本身的引用，以支持链式调用。
         * @note 窄字符可以输出到任意流，所以这里有个 .c_str()。
         */
        template <class CharT, class Traits>
        friend std::basic_ostream<CharT, Traits> &
        operator<<(std::basic_ostream<CharT, Traits> &os, const JsonValue &jv) {
            return os << jv.dump().c_str();
        }

    private:
        Type type_;
        union {
            bool         bool_;
            double       num_;
            std::string *str_;
            JsonArray   *arr_;
            JsonObject  *obj_;
        };

        /**
         * @brief 将字符串按 JSON 转义规则写入输出流。
         * @param os 输出流。
         * @param s 待转义的字符串。
         */
        static void escape(std::ostream &os, const std::string &s) {
            os << '"';
            for (char c : s) {
                switch (c) {
                case '"':
                    os << "\\\"";
                    break;
                case '\\':
                    os << "\\\\";
                    break;
                case '\n':
                    os << "\\n";
                    break;
                case '\r':
                    os << "\\r";
                    break;
                case '\t':
                    os << "\\t";
                    break;
                case '\b':
                    os << "\\b";
                    break;
                case '\f':
                    os << "\\f";
                    break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20) {
                        char buf[8];
                        snprintf(buf,
                            sizeof(buf),
                            "\\u%04x",
                            static_cast<unsigned>(
                                static_cast<unsigned char>(c)));
                        os << buf;
                    } else
                        os << c;
                }
            }
            os << '"';
        }

        /**
         * @brief 将当前 JSON 值递归序列化到输出流。
         * @param os 输出流。
         */
        void serialize(std::ostream &os) const {
            switch (type_) {
            case Type::Null:
                os << "null";
                break;
            case Type::Bool:
                os << (bool_ ? "true" : "false");
                break;
            case Type::Number: {
                char buf[32];
                int  n = 0;
                if (std::isnan(num_)) {
                    memcpy(buf, "nan", 3);
                    n = 3;
                } else if (std::isinf(num_)) {
                    if (num_ < 0) {
                        memcpy(buf, "-inf", 4);
                        n = 4;
                    } else {
                        memcpy(buf, "inf", 3);
                        n = 3;
                    }
                } else {
                    for (int prec = 1; prec <= 17; ++prec) {
                        n = snprintf(buf, sizeof(buf), "%.*g", prec, num_);
                        if (n <= 0 || static_cast<size_t>(n) >= sizeof(buf)) {
                            n = snprintf(buf, sizeof(buf), "%.17g", num_);
                            break;
                        }
                        char  *end    = nullptr;
                        double parsed = strtod(buf, &end);
                        if (end == buf + n && parsed == num_) break;
                    }
                    for (int i = 0; i < n; ++i)
                        if (buf[i] == ',') buf[i] = '.';
                }
                os.write(buf, n);
                break;
            }
            case Type::String:
                escape(os, *str_);
                break;
            case Type::Array: {
                os << '[';
                for (size_t i = 0; i < arr_->size(); ++i) {
                    if (i) os << ", ";
                    (*arr_)[i].serialize(os);
                }
                os << ']';
                break;
            }
            case Type::Object: {
                os << '{';
                bool first = true;
                for (const auto &p : *obj_) {
                    if (!first) os << ", ";
                    first = false;
                    escape(os, p.first);
                    os << ": ";
                    p.second.serialize(os);
                }
                os << '}';
                break;
            }
            }
        }
    };

    /**
     * @enum TokenType
     * @brief JSON 词法单元的类型枚举，表示词法分析过程中可能产生的各种记号。
     */
    enum class TokenType {
        Null,
        True,
        False,
        Number,
        String,
        LBrace,
        RBrace,
        LBracket,
        RBracket,
        Comma,
        Colon,
        End
    };

    /// @brief JSON 词法单元，表示词法分析产生的一个记号及其携带的字面量数据。
    struct JsonToken {
        TokenType   type;      ///< 记号类型。
        std::string str_value; ///< 字符串类型记号所携带的文本内容。
        double      num_value; ///< 数字类型记号所携带的数值。

        /// @brief 构造函数
        JsonToken(TokenType type_ = TokenType::Null,
            std::string     str   = "",
            double          num   = 0) :
            type(type_), str_value(std::move(str)), num_value(num) {}
    };

    /**
     * @brief JSON 词法分析器，将 JSON 源文本逐个解析为词法单元 (JsonToken)。
     */
    class JsonLexer {
    public:
        /// @brief 构造函数。
        explicit JsonLexer(const std::string &src) : src_(src), pos_(0) {}

        /**
         * @brief 下一个 Token。
         * @throw ValueError 解析失败时抛出。
         */
        JsonToken next() {
            skip_whitespace();
            if (pos_ >= src_.size()) return {TokenType::End};
            char c = src_[pos_];
            switch (c) {
            case '{':
                ++pos_;
                return {TokenType::LBrace};
            case '}':
                ++pos_;
                return {TokenType::RBrace};
            case '[':
                ++pos_;
                return {TokenType::LBracket};
            case ']':
                ++pos_;
                return {TokenType::RBracket};
            case ',':
                ++pos_;
                return {TokenType::Comma};
            case ':':
                ++pos_;
                return {TokenType::Colon};
            case '"':
                return parse_string();
            }
            if (c == 't' || c == 'f' || c == 'n') return parse_literal();
            if (c == '-' || (c >= '0' && c <= '9')) return parse_number();
            throw ValueError(std::string("unexpected char '") + c + "' at pos "
                             + std::to_string(pos_));
        }

    private:
        const std::string &src_; ///< JSON 源文本的常量引用
        size_t             pos_; ///< 当前解析位置。

        /// @brief 跳过空白字符。
        void skip_whitespace() {
            while (pos_ < src_.size()) {
                char c = src_[pos_];
                if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
                    ++pos_;
                else
                    break;
            }
        }

        /**
         * @brief 解析字符串 Token。
         * @return 字符串类型的 Token。
         * @throw ValueError 当字符串未终止或转义非法时抛出。
         */
        JsonToken parse_string() {
            ++pos_;
            std::string result;
            while (pos_ < src_.size()) {
                char c = src_[pos_++];
                if (c == '"') return {TokenType::String, result};
                if (c == '\\') {
                    if (pos_ >= src_.size())
                        throw ValueError("unterminated escape");
                    char esc = src_[pos_++];
                    switch (esc) {
                    case '"':
                        result += '"';
                        break;
                    case '\\':
                        result += '\\';
                        break;
                    case '/':
                        result += '/';
                        break;
                    case 'n':
                        result += '\n';
                        break;
                    case 't':
                        result += '\t';
                        break;
                    case 'r':
                        result += '\r';
                        break;
                    case 'b':
                        result += '\b';
                        break;
                    case 'f':
                        result += '\f';
                        break;
                    case 'u': {
                        auto hex4 = [&]() -> unsigned int {
                            if (pos_ + 4 > src_.size())
                                throw ValueError("bad \\u escape");
                            unsigned int v = 0;
                            for (int i = 0; i < 4; ++i) {
                                char h = src_[pos_++];
                                v <<= 4;
                                if (h >= '0' && h <= '9')
                                    v |= (h - '0');
                                else if (h >= 'a' && h <= 'f')
                                    v |= (h - 'a' + 10);
                                else if (h >= 'A' && h <= 'F')
                                    v |= (h - 'A' + 10);
                                else
                                    throw ValueError(
                                        "invalid hex digit in \\u escape");
                            }
                            return v;
                        };
                        unsigned int cp = hex4();
                        if (cp >= 0xD800 && cp <= 0xDBFF) {
                            if (pos_ + 1 >= src_.size() || src_[pos_] != '\\'
                                || src_[pos_ + 1] != 'u')
                                throw ValueError("expected low surrogate after "
                                                 "high surrogate");
                            pos_ += 2;
                            unsigned int lo = hex4();
                            if (lo < 0xDC00 || lo > 0xDFFF)
                                throw ValueError("invalid low surrogate");
                            cp = 0x10000 + ((cp - 0xD800) << 10)
                                 + (lo - 0xDC00);
                        } else if (cp >= 0xDC00 && cp <= 0xDFFF)
                            throw ValueError("unexpected low surrogate");
                        if (cp < 0x80)
                            result += static_cast<char>(cp);
                        else if (cp < 0x800) {
                            result += static_cast<char>(0xC0 | (cp >> 6));
                            result += static_cast<char>(0x80 | (cp & 0x3F));
                        } else if (cp < 0x10000) {
                            result += static_cast<char>(0xE0 | (cp >> 12));
                            result
                                += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                            result += static_cast<char>(0x80 | (cp & 0x3F));
                        } else {
                            result += static_cast<char>(0xF0 | (cp >> 18));
                            result += static_cast<char>(
                                0x80 | ((cp >> 12) & 0x3F));
                            result
                                += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                            result += static_cast<char>(0x80 | (cp & 0x3F));
                        }
                        break;
                    }
                    default:
                        throw ValueError("invalid escape");
                    }
                } else {
                    result += c;
                }
            }
            throw ValueError("unterminated string");
        }

        /**
         * @brief 解析字面量 Token (true、false、null)。
         * @return 对应类型的 Token。
         * @throw ValueError 当字面量非法时抛出。
         */
        JsonToken parse_literal() {
            if (src_.compare(pos_, 4, "true") == 0) {
                pos_ += 4;
                return {TokenType::True};
            }
            if (src_.compare(pos_, 5, "false") == 0) {
                pos_ += 5;
                return {TokenType::False};
            }
            if (src_.compare(pos_, 4, "null") == 0) {
                pos_ += 4;
                return {TokenType::Null};
            }
            throw ValueError("invalid literal at pos " + std::to_string(pos_));
        }

        /**
         * @brief 解析数字 Token。
         * @return 数字类型的 Token。
         * @throw ValueError 当数字格式非法或超出可表示范围时抛出。
         */
        JsonToken parse_number() {
            size_t start = pos_;
            if (src_[pos_] == '-') ++pos_;
            while (pos_ < src_.size() && isdigit(src_[pos_])) ++pos_;
            if (pos_ < src_.size() && src_[pos_] == '.') {
                ++pos_;
                while (pos_ < src_.size() && isdigit(src_[pos_])) ++pos_;
            }
            if (pos_ < src_.size()
                && (src_[pos_] == 'e' || src_[pos_] == 'E')) {
                ++pos_;
                if (pos_ < src_.size()
                    && (src_[pos_] == '+' || src_[pos_] == '-'))
                    ++pos_;
                while (pos_ < src_.size() && isdigit(src_[pos_])) ++pos_;
            }
            try {
                double val = std::stod(src_.substr(start, pos_ - start));
                return {TokenType::Number, "", val};
            } catch (...) {
                throw ValueError("not a number");
            }
        }
    };

    /**
     * @brief JSON 语法分析器，将词法单元流递归下降解析为 JsonValue。
     */
    class JsonParser {
    public:
        /**
         * @brief 构造函数，使用源文本初始化词法分析器并预读第一个 Token。
         * @param src JSON 源文本。
         */
        explicit JsonParser(const std::string &src) : lexer_(src) { advance(); }

        /**
         * @brief 解析整个 JSON 文本并返回根 JsonValue。
         * @return 解析得到的 JsonValue。
         * @throw ValueError 当 JSON 格式非法或存在尾随多余内容时抛出。
         */
        JsonValue parse() {
            JsonValue v = parse_value();
            if (cur_.type != TokenType::End)
                throw ValueError("trailing garbage after JSON");
            return v;
        }

    private:
        JsonLexer lexer_; ///< 词法分析器。
        JsonToken cur_;   ///< 当前预读的 Token。

        /// @brief 前进到下一个 Token。
        void advance() { cur_ = lexer_.next(); }

        /**
         * @brief 解析任意类型的 JSON 值。
         * @return 解析得到的 JsonValue。
         * @throw ValueError 当遇到非预期的 Token 时抛出。
         */
        JsonValue parse_value() {
            switch (cur_.type) {
            case TokenType::Null:
                advance();
                return JsonValue(nullptr);
            case TokenType::True:
                advance();
                return JsonValue(true);
            case TokenType::False:
                advance();
                return JsonValue(false);
            case TokenType::Number: {
                double n = cur_.num_value;
                advance();
                return JsonValue(n);
            }
            case TokenType::String: {
                std::string s = cur_.str_value;
                advance();
                return JsonValue(std::move(s));
            }
            case TokenType::LBracket:
                return parse_array();
            case TokenType::LBrace:
                return parse_object();
            default:
                throw ValueError("unexpected token in parseValue");
            }
        }

        /**
         * @brief 解析 JSON 数组。
         * @return 数组类型的 JsonValue。
         * @throw ValueError 当数组语法非法时抛出。
         */
        JsonValue parse_array() {
            advance();
            JsonArray arr;
            if (cur_.type == TokenType::RBracket) {
                advance();
                return JsonValue(std::move(arr));
            }
            while (true) {
                arr.push_back(parse_value());
                if (cur_.type == TokenType::Comma) {
                    advance();
                    continue;
                }
                if (cur_.type == TokenType::RBracket) {
                    advance();
                    break;
                }
                throw ValueError("expected ',' or ']' in array");
            }
            return JsonValue(std::move(arr));
        }

        /**
         * @brief 解析 JSON 对象。
         * @return 对象类型的 JsonValue。
         * @throw ValueError 当对象语法非法时抛出。
         */
        JsonValue parse_object() {
            advance();
            JsonObject obj;
            if (cur_.type == TokenType::RBrace) {
                advance();
                return JsonValue(std::move(obj));
            }
            while (true) {
                if (cur_.type != TokenType::String)
                    throw ValueError("expected string key in object");
                std::string key = cur_.str_value;
                advance();
                if (cur_.type != TokenType::Colon)
                    throw ValueError("expected ':' after key");
                advance();
                obj[key] = parse_value();
                if (cur_.type == TokenType::Comma) {
                    advance();
                    continue;
                }
                if (cur_.type == TokenType::RBrace) {
                    advance();
                    break;
                }
                throw ValueError("expected ',' or '}' in object");
            }
            return JsonValue(std::move(obj));
        }
    };

    /**
     * @brief 解析 JSON 文本并返回对应的 JsonValue。
     * @param src JSON 源文本。
     * @return 解析得到的 JsonValue。
     * @throw ValueError 当 JSON 格式非法时抛出。
     */
    inline JsonValue parse_json(const std::string &src) {
        return JsonParser(src).parse();
    }
}

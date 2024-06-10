#include "structure/spk_iostream_tester.hpp"

TEST_F(IOStreamTest, BasicOutput)
{
    ioStream << L"Hello, World!";
    ASSERT_EQ(testStream1.str(), L"Hello, World!") << "The output stream should contain 'Hello, World!'";
}

TEST_F(IOStreamTest, PrefixOutput)
{
    ioStream.setPrefix(L"[Prefix] ");
    ioStream << L"Hello, World!";
    ASSERT_EQ(testStream1.str(), L"[Prefix] Hello, World!") << "The output stream should contain '[Prefix] Hello, World!'";
}

TEST_F(IOStreamTest, NewLineHandling)
{
    ioStream.setPrefix(L"[Prefix] ");
    ioStream << L"Hello," << std::endl << L"World!";
    ASSERT_EQ(testStream1.str(), L"[Prefix] Hello,\n[Prefix] World!") << "The output stream should contain '[Prefix] Hello,\\n[Prefix] World!'";
}

TEST_F(IOStreamTest, MultipleLines)
{
    ioStream.setPrefix(L"[Prefix] ");
    ioStream << L"Line 1" << std::endl << L"Line 2" << std::endl << L"Line 3";
    ASSERT_EQ(testStream1.str(), L"[Prefix] Line 1\n[Prefix] Line 2\n[Prefix] Line 3") << "The output stream should contain '[Prefix] Line 1\\n[Prefix] Line 2\\n[Prefix] Line 3'";
}

TEST_F(IOStreamTest, IntegerOutput)
{
    ioStream << 42;
    ASSERT_EQ(testStream1.str(), L"42") << "The output stream should contain '42'";
}

TEST_F(IOStreamTest, MixedTypeOutput)
{
    ioStream << L"Number: " << 42 << L", Text: " << L"Hello";
    ASSERT_EQ(testStream1.str(), L"Number: 42, Text: Hello") << "The output stream should contain 'Number: 42, Text: Hello'";
}

TEST_F(IOStreamTest, OutputWithManipulators)
{
    ioStream << L"Line 1" << std::endl << L"Line 2";
    ASSERT_EQ(testStream1.str(), L"Line 1\nLine 2") << "The output stream should contain 'Line 1\\nLine 2'";
}

TEST_F(IOStreamTest, StreamResetAfterNewLine)
{
    ioStream.setPrefix(L"[Prefix] ");
    ioStream << L"Hello, World!" << std::endl;
    ASSERT_EQ(testStream1.str(), L"[Prefix] Hello, World!\n") << "The output stream should contain '[Prefix] Hello, World!\\n'";

    ioStream << L"New Line";
    ASSERT_EQ(testStream1.str(), L"[Prefix] Hello, World!\n[Prefix] New Line") << "The output stream should contain '[Prefix] Hello, World!\\n[Prefix] New Line'";
}

TEST_F(IOStreamTest, RedirectStream)
{
    ioStream << L"Initial output.";
    ASSERT_EQ(testStream1.str(), L"Initial output.") << "The initial output should be in the first stream";

    ioStream.redirect(testStream2);
    ioStream << L" Redirected output.";
    ASSERT_EQ(testStream2.str(), L" Redirected output.") << "The redirected output should be in the second stream";
    ASSERT_EQ(testStream1.str(), L"Initial output.") << "The first stream should remain unchanged after redirection";
}
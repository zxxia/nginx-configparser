#include "gtest/gtest.h"
#include "config_parser.h"

TEST(NginxConfigParserTest, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("example_config", &out_config);

  EXPECT_TRUE(success);
}

// test fixture used in following test
class NginxConfigParser_Test:public::testing::Test{
protected:
	NginxConfig out_config;
	NginxConfigParser parser;
	bool parse_test_str(const std::string input_config_str){
		std::stringstream input_config_str_stream(input_config_str);
		return parser.Parse(&input_config_str_stream, &out_config);
	}
};


// a simple configuration test
TEST_F(NginxConfigParser_Test, SimpleConfig) {
	EXPECT_TRUE(parse_test_str("foo bar;"));
	EXPECT_EQ(1, out_config.statements_.size());
	EXPECT_EQ(2, out_config.statements_[0]->tokens_.size());
	EXPECT_EQ("foo", out_config.statements_[0]->tokens_[0]);
	EXPECT_EQ("bar", out_config.statements_[0]->tokens_[1]);
}

// test unbalanced curly brackets(no end brackets)
TEST_F(NginxConfigParser_Test, UnbalancedBlock1) {
	EXPECT_FALSE(parse_test_str("server\n{ foo bar;"));
}


// test unbalanced curly brackets(no beginning brackets)
TEST_F(NginxConfigParser_Test, UnbalancedBlock2) {
	EXPECT_FALSE(parse_test_str("server\nfoo bar; }"));
}

// test invalid configuration expression e.g. no semicolon
TEST_F(NginxConfigParser_Test, InvalidExpr) {
	EXPECT_FALSE(parse_test_str("foo bar"));
}

// test comment in configuration expression
TEST_F(NginxConfigParser_Test, Comment) {
	EXPECT_TRUE(parse_test_str("foo bar; # This is a comment"));
	EXPECT_EQ(1, out_config.statements_.size());
	EXPECT_EQ(2, out_config.statements_[0]->tokens_.size());
}

// test nested curly brackets
TEST_F(NginxConfigParser_Test, NestedBlock) {
	EXPECT_TRUE(parse_test_str("foobar {server; \nListen 80;\n name {abcd;}\n root {foobar;}}"));
}


// test flipped order of curly brackets
TEST_F(NginxConfigParser_Test, flippedBrackets) {
	EXPECT_FALSE(parse_test_str("}sever \n listen 80;{"));
}

// test no expression before semicolon
TEST_F(NginxConfigParser_Test, independentSemicolon){
	EXPECT_FALSE(parse_test_str("foo;\n;\nbar;"));
}

// test odd number of double quotes
TEST_F(NginxConfigParser_Test, oddDoubleQuote){
	EXPECT_FALSE(parse_test_str("foo;\n \"\"bar;\";"));
}

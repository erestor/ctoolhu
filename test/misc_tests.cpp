#include <ctoolhu/event/firer.hpp>
#include <ctoolhu/event/free_subscriber.hpp>
#include <ctoolhu/filesystem/directory_creator.hpp>
#include <ctoolhu/maths/comparer.hpp>
#include <ctoolhu/property_tree/ptree_ext.hpp>
#include <ctoolhu/visitor/visitable.hpp>
#include <ctoolhu/visitor/visitor.hpp>
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

TEST(ComparerTest, FloatWithDefaultPrecision)
{
	using C = Ctoolhu::Maths::Comparer<>;
	EXPECT_TRUE(C::equal(1.0f, 1.000001f));
	EXPECT_FALSE(C::equal(1.0f, 1.0001f));
	EXPECT_TRUE(C::less(1.0f, 2));
	EXPECT_TRUE(C::greaterOrEqual(1.0f, 1.000001));
}

TEST(ComparerTest, DoubleWithHighPrecision)
{
	using C = Ctoolhu::Maths::Comparer<double, 12>;
	EXPECT_TRUE(C::equal(1.0, 1.0 + 1e-13));
	EXPECT_FALSE(C::equal(1.0, 1.0 + 1e-11));
}

TEST(PropertyTreeTest, CreateArray_SkipsEmptyElements)
{
	using boost::property_tree::ptree;
	std::vector<int> const values{1, 0, 2};
	auto const tree = Ctoolhu::PropertyTree::create_array(values, [](int i, ptree &pt) {
		if (i)
			pt.put("v", i);
	});
	ASSERT_EQ(tree.size(), 2u);
	EXPECT_EQ(tree.front().second.get<int>("v"), 1);
	EXPECT_EQ(tree.back().second.get<int>("v"), 2);
}

namespace {

	struct TestEvent {};

	struct Target {
		int hits = 0;
		void on(TestEvent *) { ++hits; }
	};

} //ns

TEST(FreeSubscriberTest, Subscribe_ReceivesEvents)
{
	Target t;
	Ctoolhu::Event::FreeSubscriber s;
	s.Subscribe(&t, &Target::on);
	Ctoolhu::Event::Fire<TestEvent>();
	EXPECT_EQ(t.hits, 1);
}

TEST(FreeSubscriberTest, Destruction_Unsubscribes)
{
	Target t;
	{
		Ctoolhu::Event::FreeSubscriber s;
		s.Subscribe(&t, &Target::on);
	}
	Ctoolhu::Event::Fire<TestEvent>();
	EXPECT_EQ(t.hits, 0);
}

TEST(FreeSubscriberTest, MoveAssignment_DisconnectsPreviousHandlers)
{
	Target t1, t2;
	Ctoolhu::Event::FreeSubscriber s1, s2;
	s1.Subscribe(&t1, &Target::on);
	s2.Subscribe(&t2, &Target::on);
	s1 = std::move(s2);
	Ctoolhu::Event::Fire<TestEvent>();
	EXPECT_EQ(t1.hits, 0);
	EXPECT_EQ(t2.hits, 1);
}

namespace {

	struct Circle;
	struct Square;

	using ShapeVisitor = Ctoolhu::Visitor::ConstVisitor<int, Circle, Square>;

	struct Shape : Ctoolhu::Visitor::VisitableRoot<ShapeVisitor> {};
	struct Circle : Ctoolhu::Visitor::Visitable<Circle, Shape, ShapeVisitor> {};
	struct Square : Ctoolhu::Visitor::Visitable<Square, Shape, ShapeVisitor> {};

	//keeps state, which is why visit isn't const
	struct CountingVisitor : ShapeVisitor {
		int circles = 0;
		int squares = 0;
		int visit(const Circle &) override { return ++circles; }
		int visit(const Square &) override { return ++squares; }
	};

} //ns

TEST(VisitorTest, Accept_DispatchesToTheRightOverload)
{
	std::vector<std::unique_ptr<Shape>> shapes;
	shapes.push_back(std::make_unique<Circle>());
	shapes.push_back(std::make_unique<Square>());
	shapes.push_back(std::make_unique<Square>());

	CountingVisitor v;
	for (auto const &s : shapes)
		s->accept(v);
	EXPECT_EQ(v.circles, 1);
	EXPECT_EQ(v.squares, 2);
}

namespace {

	class EnsureDirectoryExistsTest : public testing::Test {

	  protected:

		void SetUp() override { std::filesystem::remove_all(_root); }
		void TearDown() override { std::filesystem::remove_all(_root); }

		const std::filesystem::path _root{std::filesystem::temp_directory_path() / "ctoolhu_test"};
	};

} //ns

TEST_F(EnsureDirectoryExistsTest, CreatesMissingParentsAndReturnsThePath)
{
	auto const dir = _root / "a" / "b";
	EXPECT_EQ(Ctoolhu::FileSystem::EnsureDirectoryExists(dir), dir);
	EXPECT_TRUE(std::filesystem::is_directory(dir));
}

TEST_F(EnsureDirectoryExistsTest, ExistingDirectory_DoesNothing)
{
	auto const dir = _root / "a";
	std::filesystem::create_directories(dir);
	EXPECT_NO_THROW(Ctoolhu::FileSystem::EnsureDirectoryExists(dir));
}

TEST_F(EnsureDirectoryExistsTest, FileOfTheSameName_Throws)
{
	std::filesystem::create_directories(_root);
	auto const file = _root / "file";
	std::ofstream{file} << "x";
	EXPECT_THROW(Ctoolhu::FileSystem::EnsureDirectoryExists(file), std::filesystem::filesystem_error);
}

TEST_F(EnsureDirectoryExistsTest, StringPath_ReturnsTheSameString)
{
	std::string const dir = (_root / "s").string();
	std::string const result = Ctoolhu::FileSystem::EnsureDirectoryExists(dir);
	EXPECT_EQ(result, dir);
	EXPECT_TRUE(std::filesystem::is_directory(dir));
}

#include <math_lib/plane.h>
#include <math_lib/projection.h>

#include <boost/qvm/vec_operations.hpp>

#include <gtest/gtest.h>

using namespace pagoda;

TEST(Plane, when_using_default_constructor_should_construct_the_xy_plane)
{
	Plane<float> p;
	EXPECT_EQ(p.GetDistanceToOrigin(), 0.0f);
	EXPECT_TRUE(p.GetNormal() == (Vec3F{0, 0, 1}));
}

TEST(Plane, when_constructing_with_normal_and_distance_should_set_the_normal_and_distance_values)
{
	Plane<float> p(Vec3F{1, 2, 3}, 4);
	EXPECT_EQ(p.GetDistanceToOrigin(), 4.0f);
	EXPECT_TRUE(p.GetNormal() == (Vec3F{1, 2, 3}));
}

TEST(Plane, when_constructing_from_point_and_normal_should_construct_the_plane_correctly)
{
	auto p = Plane<float>::FromPointAndNormal(Vec3F{1, 0, 0}, Vec3F{0, 1, 0});
	EXPECT_EQ(p.GetDistanceToOrigin(), 0.0f);
	EXPECT_TRUE(p.GetNormal() == (Vec3F{0, 1, 0}));
}

TEST(Plane, when_constructing_from_three_points_should_construct_the_plane_correctly)
{
	auto p = Plane<float>::FromPoints(Vec3F{1, 0.5, 0}, Vec3F{0, 0.5, 0}, Vec3F{0, 0.5, 1});
	EXPECT_EQ(p.GetDistanceToOrigin(), 0.5f);
	EXPECT_TRUE(p.GetNormal() == (Vec3F{0, 1, 0}));
}

TEST(Plane, when_constructing_from_a_point_and_two_vectors_should_construct_the_plane_correctly)
{
	auto p = Plane<float>::FromPointAndVectors(Vec3F{1, 1, 1}, Vec3F{1, 0, 0}, Vec3F{0, 1, 0});
	EXPECT_EQ(p.GetDistanceToOrigin(), 1.0f);
	EXPECT_TRUE(p.GetNormal() == (Vec3F{0, 0, 1}));
}

TEST(Plane, when_getting_vectors_contained_in_the_plane_should_get_vectors_orthogonal_to_the_normal)
{
	Plane<float> p(Vec3F{1, 2, 3}, 4);
	EXPECT_EQ(boost::qvm::dot(p.GetVector(), (Vec3F{1, 2, 3})), 0.0f);
	EXPECT_EQ(boost::qvm::dot(p.GetVector2(), (Vec3F{1, 2, 3})), 0.0f);
}

TEST(Plane, when_getting_points_contained_in_the_plane_should_have_zero_distance_to_the_plane)
{
	Plane<float> plane(Vec3F{0, 0, 1}, 4);
	auto p1 = plane.GetPoint();
	auto p2 = plane.GetPoint2();

	EXPECT_TRUE(projection(p1, plane) == p1);
	EXPECT_TRUE(projection(p2, plane) == p2);
}

TEST(Plane, when_a_getting_the_side_of_the_plane_a_point_is_in_should_return_the_right_value)
{
	Plane<float> plane = Plane<float>::FromPointAndNormal(Vec3F{0, 0, 0}, Vec3F{1, 0, 0});

	EXPECT_EQ(plane.GetPlaneSide((Vec3F{1, 0, 0})), Plane<float>::PlaneSide::Front);
	EXPECT_EQ(plane.GetPlaneSide((Vec3F{0, 0, 0})), Plane<float>::PlaneSide::Contained);
	EXPECT_EQ(plane.GetPlaneSide((Vec3F{-1, 0, 0})), Plane<float>::PlaneSide::Back);
}

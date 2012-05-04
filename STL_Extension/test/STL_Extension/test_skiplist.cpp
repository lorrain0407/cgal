#define BOOST_TEST_MODULE Skiplist
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <string>
#include <boost/range/algorithm.hpp>
#include <boost/assign.hpp>

#include <CGAL/Skiplist.h>

typedef CGAL::Skiplist<int> skip;
typedef skip::skip_iterator skip_iterator;
typedef skip::all_iterator all_iterator;

using namespace boost::assign;

struct Fixture {
  Fixture() {
    skips += 1, 2, 3, 4, 5, 6, 7;
    all = skips;
    l.insert(l.all_begin(), skips.begin(), skips.end());
  }
  std::vector<int> all;
  std::vector<int> skips;
  skip l;
};

BOOST_AUTO_TEST_CASE( test_construction )
{
  std::vector<int> v;
  v += 1, 2, 3, 4, 5, 6, 7;
  skip l(v.begin(), v.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.all_begin(), l.all_end(), 
                                v.begin(), v.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                v.begin(), v.end());
}

BOOST_FIXTURE_TEST_CASE( test_insert, Fixture )
{
  l.insert(l.all_end(), 8);
  l.insert(l.all_end(), 9);
  all += 8, 9;
  // 8 and 9 should be included in the all range
  BOOST_CHECK_EQUAL_COLLECTIONS(l.all_begin(), l.all_end(), 
                                all.begin(), all.end());
  // but should not be part of the skipped set
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                skips.begin(), skips.end());

  // clear and try again
  l.clear();
  BOOST_CHECK_EQUAL(l.all_size(), 0);
  BOOST_CHECK_EQUAL(l.skip_size(), 0);
  l.insert(l.all_begin(), all.begin(), all.end());
  skips += 8, 9;
  BOOST_CHECK_EQUAL_COLLECTIONS(l.all_begin(), l.all_end(), 
                                all.begin(), all.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                skips.begin(), skips.end());
  
  // the same goes for inserting at an arbitrary position
  skip::all_iterator pos = boost::next(l.all_begin(), 3);
  l.insert(boost::next(l.all_begin(), 3)
           , 20);
  all.insert(boost::next(all.begin(), 3)
             , 20);
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                skips.begin(), skips.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.all_begin(), l.all_end(), 
                                all.begin(), all.end());
}

BOOST_FIXTURE_TEST_CASE( test_single_skip, Fixture )
{
  for(skip::all_iterator it = l.all_begin(); it != l.all_end(); ++it) { 
    BOOST_CHECK(!l.is_skipped(it));
  }
  
  // skip somewhere in between and at the end
  // skip 2 and 7
  l.skip(boost::next(l.all_begin()));
  BOOST_CHECK(l.is_skipped(boost::next(l.all_begin())));
  skips.erase(std::remove(skips.begin(), skips.end(), 2), skips.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(),
                                skips.begin(), skips.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.all_begin(), l.all_end(), 
                                all.begin(), all.end());
  l.skip(boost::prior(l.all_end()));
  BOOST_CHECK(l.is_skipped(boost::prior(l.all_end())));
  skips.erase(std::remove(skips.begin(), skips.end(), 7), skips.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(),
                                skips.begin(), skips.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.all_begin(), l.all_end(), 
                                all.begin(), all.end());
  // skip the beginning
  l.skip(l.all_begin());
  BOOST_CHECK(l.is_skipped(l.all_begin()));
  skips.erase(std::remove(skips.begin(), skips.end(), 1), skips.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                skips.begin(), skips.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.all_begin(), l.all_end(), 
                                all.begin(), all.end());
}

BOOST_FIXTURE_TEST_CASE( test_range_skip, Fixture )
{
  // drop all from 2 up to 4
  l.skip(boost::next(l.all_begin()), boost::next(l.all_begin(), 3));
  skips.erase(std::remove(skips.begin(), skips.end(), 2), skips.end());
  skips.erase(std::remove(skips.begin(), skips.end(), 3), skips.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                skips.begin(), skips.end());
  // drop 6 and 7
  l.skip(boost::prior(l.all_end(), 2), l.all_end());
  skips.erase(std::remove(skips.begin(), skips.end(), 6), skips.end());
  skips.erase(std::remove(skips.begin(), skips.end(), 7), skips.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                skips.begin(), skips.end());
}

BOOST_FIXTURE_TEST_CASE( test_to, Fixture )
{
  BOOST_CHECK(l.to_all(l.skip_end()) == l.all_end());
  l.skip(l.all_begin());
  BOOST_CHECK(l.to_all(l.skip_end()) == l.all_end());
  BOOST_CHECK(*l.to_all(l.skip_begin()) == 2);
}

BOOST_FIXTURE_TEST_CASE( skip_all_case, Fixture )
{
  l.skip(l.all_begin(), l.all_end());
  skips.clear();
  BOOST_CHECK_EQUAL(l.all_size(), all.size());
  BOOST_CHECK_EQUAL(l.skip_size(), 0);
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                skips.begin(), skips.end());
}

BOOST_AUTO_TEST_CASE( test_continous_insert )
{
  
}

BOOST_FIXTURE_TEST_CASE( test_unskip, Fixture )
{
  // skip 2 and 3
  l.skip(boost::next(l.all_begin()), boost::next(l.all_begin(), 3));
  skips.erase(std::remove(skips.begin(), skips.end(), 2), skips.end());
  skips.erase(std::remove(skips.begin(), skips.end(), 3), skips.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                skips.begin(), skips.end());

  // unskip 2
  l.unskip(boost::next(l.skip_begin()), boost::next(l.all_begin()));
  skips.insert(boost::next(skips.begin()), 2);
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                skips.begin(), skips.end());
  // unskip 3
  l.unskip(boost::next(l.skip_begin(), 2), boost::next(l.all_begin(), 2));
  skips.insert(boost::next(skips.begin(), 2), 3);
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                skips.begin(), skips.end());

}

BOOST_FIXTURE_TEST_CASE( test_push, Fixture )
{
  using namespace boost::assign;

  l.push_back(8);
  l.push_back(9);
  all.push_back(8); all.push_back(9);
  skips.push_back(8); skips.push_back(9);
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                skips.begin(), skips.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.all_begin(), l.all_end(), 
                                all.begin(), all.end());
 
  l.push_front(0); l.push_front(-1);
  all.insert(all.begin(), 0);
  all.insert(all.begin(), -1);
  skips.insert(skips.begin(), 0);
  skips.insert(skips.begin(), -1);

  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                skips.begin(), skips.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.all_begin(), l.all_end(), 
                                all.begin(), all.end());

  // check how it works with skipped elements
  l.skip(l.all_begin(), boost::next(l.all_begin(), 4));
  skips.erase(skips.begin(), boost::next(skips.begin(), 4));
  l.push_front(20);
  l.push_front(21);
  skips.insert(skips.begin(), 20); 
  skips.insert(skips.begin(), 21);
  all.insert(all.begin(), 20); 
  all.insert(all.begin(), 21);
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                skips.begin(), skips.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.all_begin(), l.all_end(), 
                                all.begin(), all.end());
}

void inc(int& i) {
  ++i;
}

BOOST_FIXTURE_TEST_CASE( test_modify, Fixture )
{
  for(all_iterator it = l.all_begin(); it != l.all_end(); ++it) { 
    BOOST_CHECK(l.modify(it, &inc));
  }
  std::for_each(all.begin(), all.end(), &inc);
  std::for_each(skips.begin(), skips.end(), &inc);
  BOOST_CHECK_EQUAL_COLLECTIONS(l.skip_begin(), l.skip_end(), 
                                skips.begin(), skips.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(l.all_begin(), l.all_end(), 
                                all.begin(), all.end());
}


// trick cgal_create_cmake_script
// int main()
// {
// }

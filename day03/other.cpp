#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace ::std;
#include <cstring>

#define D(e) cerr << e << endl
#define E(e) #e << " = " << (e)
#define COMMA << ", "

inline int binary ( const string &s ) {
    int result = 0;
    for ( auto c : s )
        result <<= 1, result |= ( c & 1 );
    return result;
}

struct bit_is_ {
    int bit, desired;
    bit_is_( int bit, int desired ) : bit(bit), desired(desired) { }
    bool operator() ( const string &s ) const { return ( s[bit] & 1 ) == desired; }
};

struct bit_is_set : public bit_is_ {
    bit_is_set( int bit ) : bit_is_( bit, 1 ) { }
};

int most_popular ( int bit, const vector<string> &values ) {
    int ones = count_if( begin(values), end(values), bit_is_set(bit) );
    D( E(bit) COMMA E(ones) COMMA E(values.size()) );
    return !!( ones >= ( ( int( values.size() ) + 1 ) / 2 ) );
}

template< typename Container, typename Predicate >
auto remove_if_not ( Container &c, const Predicate &p ) {
    return remove_if ( begin(c), end(c), [&] ( const auto &value ) { return ! p( value ); } );
}

void filter_numbers ( vector<string> &numbers, bool useLeastPopular = false ) {
    D( E(useLeastPopular) );
    for ( int bit = 0; numbers.size() > 1; ++bit ) {
        int mostPopular = most_popular( bit, numbers );
        D( E(bit) COMMA E(numbers.size()) COMMA E(mostPopular) );
        auto ri = remove_if_not(
            numbers,
            bit_is_(
                bit,
                int ( useLeastPopular ^ mostPopular )
            )
        );
        numbers.erase( ri, end (numbers) );
    }
}

int main ( int, const char *[] ) {
    ios_base::sync_with_stdio(false);
    cin.tie(0);

// read all the numbers in (because it looks like we need to iterate over them more than once)
    vector<string> report;
    for ( string number; cin >> number; report.emplace_back( move(number) ) )
        ;
    D( E(report.size()) COMMA E(report.front().length()) );

// oxygen generator rating
    vector<string> ogv (report);
    filter_numbers( ogv );
    int ogr = binary( ogv.front() );

// CO2 scrubber rating
    vector<string> csv (report);
    filter_numbers( csv, true );
    int csr = binary( csv.front() );

// life support rating
    int lsr = ogr * csr;

    D( E(ogr) COMMA E(csr) COMMA E(lsr) );
    cout << lsr << endl;

    return EXIT_SUCCESS;
}
#include <bits/stdc++.h>
#define ll long long
#define ld long double
#define X first
#define Y second
#define vi vector<ll>
#define int ll

using namespace std;



void fast() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
}

template<typename T>
std::istream& operator>>(std::istream &in, std::vector<T> &a) {
    for (T &x : a) {
        in >> x;
    }
    return in;
}

template<typename T>
std::ostream& operator<<(std::ostream &out, std::vector<T> &a) {
    for (T &x : a) {
        out << x << ' ';
    }
    return out;
}

template<typename T>
std::ostream& operator<<(std::ostream &out, std::pair<T, T> &a) {
    cout << "( " << a.first <<", " << a.second << ")";
}


template<typename T>
std :: ostream & operator << (std :: ostream &out, set <T> &st){

    for(auto &x : st){
        cout << x <<' ';
    }
    return out;
}

const ll INF = 1e18, SIZE = 1e6;
const ld EPS = 1e-8;
const std::string yes = "YES\n", no = "NO\n";


vector<vector<ll>> v(SIZE);
vector<bool> used(SIZE);
vector<ll>  dp(SIZE, 0), sz(SIZE, 0);
vector<ll> global_a;

void dfs(ll cur,ll x){

    used[cur] = 1;
    dp[cur] = global_a[cur];
    for(auto elem : v[cur]){
        if (used[elem] == 0){
            dfs(elem, x);
            dp[cur] ^= dp[elem];
            sz[cur] += sz[elem];
        }
    }

    if (sz[cur] == 0 && dp[cur] == x) sz[cur]++;
    if (sz[cur] && dp[cur] == 0) sz[cur]++;
}


void solve(){

    ll n, m;
    cin >> n >> m;

    vector<ll> a(n);
    vector<ll> cnt(n + m + 1, 0), l(n + m + 1, 0);
    cin >> a;

    for(ll i = 1; i <= m; i++){
        ll x, y;
        cin >> x >> y;
        x--;
        ll tmpCnt = i - l[a[x]];
        cnt[a[x]] += tmpCnt;
        l[y] = i;
        a[x] = y;
       // cout <<"a" << a << "\n";
    }

    for(ll i = 0; i < n; i++){
        ll tmpCnt = m + 1 - l[a[i]];
        cnt[a[i]] += tmpCnt;
    }

    for(ll i = 1; i < n + m + 1; i++){
     //   cout << i << "  " << cnt[i] << "\n";
    }

    ll ans = 2*n* (m * (m + 1))/2;

    for(ll i = 1; i <= n + m; i++){

        if(cnt[i])
            ans -= (cnt[i] * (cnt[i] - 1)) / 2;
    }

    cout << ans << "\n";
}





signed main(){

   //freopen ("control.in","r",stdin);
   //freopen ("control.out","w",stdout);

   fast();
   ll T = 1;
   cin >> T;
    while(T--){
        solve();
    }

}


/*

4 4
+-+ +-+
| |   |
+ +-+-+
|   |
+-+-+-+
|     |
+-+-+-+
    */


/*
3 5
+-+-+-+-+
| |   | |
+-+-+-+ +
  | | | |
+-+-+ +-+
Вывод
E
FLRLR
Ответ
E
FLRLR

*/


/*

2 9
+ +-+-+-+-+ +-+-+
| | | | | | | | |
+-+-+-+ +-+-+-+-+
Вывод
E
FFLRFRLFFL
Ответ
E
FFLRFRLFFL

*/

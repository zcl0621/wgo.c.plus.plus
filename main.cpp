#include <iostream>
#include <vector>
#include <string>
#include "game.cpp"

// Convert SGF coordinate (like "qd") to x,y coordinates (0-18)
// 'a' is left, 's' is right for x
// 'a' is top, 's' is bottom for y
std::pair<int, int> sgfToCoord(const std::string& sgf) {
    if (sgf.length() < 2) return {-1, -1};
    int x = sgf[0] - 'a';
    int y = sgf[1] - 'a';
    return {x, y};
}

int main() {
    // Initialize a 19x19 game
    auto game = Game(19);
    
    // Game info
    std::cout << "Replaying: 第10届LG杯世界棋王赛八强赛 (10th LG Cup World Baduk Championship Quarterfinals)";
    std::cout << "古力 (Black) vs 周鹤洋 (White)" << std::endl;
    std::cout << "Komi: 6.5" << std::endl;
    std::cout << "Result: B+R (Black wins by resignation)" << std::endl;
    std::cout << "Date: 2005-10-17" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    // SGF moves from the record
    std::vector<std::pair<std::string, Color>> moves = {
        {"qd", BLACK}, {"dd", WHITE}, {"pq", BLACK}, {"dp", WHITE}, {"fc", BLACK}, {"cf", WHITE},
        {"jc", BLACK}, {"oc", WHITE}, {"pc", BLACK}, {"od", WHITE}, {"qf", BLACK}, {"lc", WHITE},
        {"db", BLACK}, {"qo", WHITE}, {"cc", BLACK}, {"ci", WHITE}, {"pl", BLACK}, {"mp", WHITE},
        {"po", BLACK}, {"pp", WHITE}, {"op", BLACK}, {"qp", WHITE}, {"qq", BLACK}, {"rq", WHITE},
        {"rr", BLACK}, {"oq", WHITE}, {"oo", BLACK}, {"rp", WHITE}, {"or", BLACK}, {"nq", WHITE},
        {"nr", BLACK}, {"mr", WHITE}, {"lq", BLACK}, {"mq", WHITE}, {"sr", BLACK}, {"rl", WHITE},
        {"jp", BLACK}, {"mn", WHITE}, {"lo", BLACK}, {"mo", WHITE}, {"om", BLACK}, {"qk", WHITE},
        {"fq", BLACK}, {"dn", WHITE}, {"ml", BLACK}, {"lm", WHITE}, {"rm", BLACK}, {"qm", WHITE},
        {"ql", BLACK}, {"rn", WHITE}, {"rk", BLACK}, {"sm", WHITE}, {"qj", BLACK}, {"hq", WHITE},
        {"hp", BLACK}, {"ns", WHITE}, {"pr", BLACK}, {"os", WHITE}, {"ps", BLACK}, {"ms", WHITE},
        {"qs", BLACK}, {"iq", WHITE}, {"ip", BLACK}, {"jq", WHITE}, {"kq", BLACK}, {"jr", WHITE},
        {"fo", BLACK}, {"gp", WHITE}, {"go", BLACK}, {"gq", WHITE}, {"fp", BLACK}, {"hn", WHITE},
        {"ho", BLACK}, {"jn", WHITE}, {"hm", BLACK}, {"in", WHITE}, {"fm", BLACK}, {"dl", WHITE},
        {"ko", BLACK}, {"ll", WHITE}, {"dq", BLACK}, {"cq", WHITE}, {"cr", BLACK}, {"dr", WHITE},
        {"eq", BLACK}, {"br", WHITE}, {"cp", BLACK}, {"bq", WHITE}, {"do", BLACK}, {"co", WHITE},
        {"ep", BLACK}, {"bo", WHITE}, {"en", BLACK}, {"em", WHITE}, {"pn", BLACK}, {"gn", WHITE},
        {"fn", BLACK}, {"gm", WHITE}, {"el", BLACK}, {"dm", WHITE}, {"gl", BLACK}, {"hl", WHITE},
        {"hk", BLACK}, {"il", WHITE}, {"ik", BLACK}, {"jl", WHITE}, {"so", BLACK}, {"rj", WHITE},
        {"sk", BLACK}, {"pk", WHITE}, {"ri", BLACK}, {"ok", WHITE}, {"qn", BLACK}, {"gk", WHITE},
        {"fl", BLACK}, {"gj", WHITE}, {"ej", BLACK}, {"dk", WHITE}, {"fj", BLACK}, {"gi", WHITE},
        {"er", BLACK}, {"cs", WHITE}, {"eh", BLACK}, {"hc", WHITE}, {"gh", BLACK}, {"hh", WHITE},
        {"gg", BLACK}, {"hg", WHITE}, {"hf", BLACK}, {"gf", WHITE}, {"if", BLACK}, {"ge", WHITE},
        {"ef", BLACK}, {"fd", WHITE}, {"gc", BLACK}, {"hd", WHITE}, {"ld", BLACK}, {"kc", WHITE},
        {"kd", BLACK}, {"jb", WHITE}, {"mc", BLACK}, {"mb", WHITE}, {"ib", BLACK}, {"jd", WHITE},
        {"ic", BLACK}, {"hb", WHITE}, {"id", BLACK}, {"ec", WHITE}, {"eb", BLACK}, {"dg", WHITE},
        {"eg", BLACK}, {"cd", WHITE}, {"fi", BLACK}, {"ii", WHITE}, {"di", BLACK}, {"je", WHITE},
        {"ie", BLACK}, {"kb", WHITE}, {"md", BLACK}, {"ia", WHITE}, {"jg", BLACK}, {"ob", WHITE},
        {"ki", BLACK}, {"jj", WHITE}, {"mj", BLACK}, {"nm", WHITE}, {"sj", BLACK}, {"mk", WHITE},
        {"ch", BLACK}, {"cj", WHITE}, {"cg", BLACK}, {"pe", WHITE}, {"ng", BLACK}, {"df", WHITE},
        {"dh", BLACK}, {"ne", WHITE}, {"mf", BLACK}, {"nj", WHITE}, {"lk", BLACK}, {"mi", WHITE},
        {"nk", BLACK}, {"nl", WHITE}, {"kj", BLACK}, {"kk", WHITE}, {"ji", BLACK}, {"jk", WHITE},
        {"pb", BLACK}, {"mk", WHITE}, {"pf", BLACK}, {"pa", WHITE}, {"qb", BLACK}, {"lg", WHITE},
        {"mh", BLACK}, {"ro", WHITE}, {"pm", BLACK}, {"jh", WHITE}, {"kg", BLACK}, {"lh", WHITE},
        {"kh", BLACK}, {"li", WHITE}, {"lf", BLACK}, {"lj", WHITE}, {"ee", BLACK}, {"ed", WHITE},
        {"gb", BLACK}, {"fg", WHITE}, {"fh", BLACK}, {"ig", WHITE}, {"ih", BLACK}, {"bc", WHITE},
        {"bb", BLACK}, {"jh", WHITE}, {"jf", BLACK}, {"he", WHITE}, {"ih", BLACK}, {"rs", WHITE},
        {"ss", BLACK}, {"jh", WHITE}, {"ke", BLACK}, {"pi", WHITE}, {"bd", BLACK}, {"be", WHITE},
        {"ac", BLACK}, {"qh", WHITE}, {"rg", BLACK}, {"qa", WHITE}, {"de", BLACK}, {"rb", WHITE},
        {"rc", BLACK}, {"pd", WHITE}, {"qc", BLACK}, {"qe", WHITE}, {"re", BLACK}, {"sc", WHITE},
        {"rd", BLACK}, {"ce", WHITE}, {"nc", BLACK}, {"nb", WHITE}, {"oa", BLACK}, {"na", WHITE},
        {"la", BLACK}, {"bg", WHITE}, {"bh", BLACK}, {"of", WHITE}, {"og", BLACK}, {"ah", WHITE},
        {"ai", BLACK}, {"rh", WHITE}, {"sh", BLACK}, {"pg", WHITE}, {"qg", BLACK}, {"ph", WHITE},
        {"pj", BLACK}, {"oi", WHITE}, {"lb", BLACK}, {"ja", WHITE}, {"nf", BLACK}, {"ag", WHITE},
        {"bi", BLACK}, {"ae", WHITE}, {"bk", BLACK}, {"sf", WHITE}, {"sg", BLACK}, {"sd", WHITE},
        {"oe", BLACK}, {"bj", WHITE}, {"ck", BLACK}, {"of", WHITE}, {"rf", BLACK}, {"dj", WHITE},
        {"aj", BLACK}, {"bm", WHITE}, {"gr", BLACK}, {"hr", WHITE}, {"hi", BLACK}, {"hj", WHITE},
        {"ih", BLACK}, {"rm", WHITE}, {"sq", BLACK}, {"jh", WHITE}, {"kr", BLACK}, {"ks", WHITE},
        {"ih", BLACK}, {"hi", WHITE}, {"is", BLACK}, {"js", WHITE}, {"ga", BLACK}, {"ha", WHITE},
        {"hs", BLACK}, {"es", WHITE}, {"oe", BLACK}, {"nd", WHITE}, {"se", BLACK}
    };

    // Replay the game
    for (size_t i = 0; i < moves.size(); i++) {
        const auto& [sgf, color] = moves[i];
        auto [x, y] = sgfToCoord(sgf);
        
        std::cout << "\nMove " << (i + 1) << ": " << (color == BLACK ? "Black" : "White") 
                  << " plays at " << sgf << " (" << x << "," << y << ")" << std::endl;
        
        auto result = game.play(x, y, color);
        
        if (std::holds_alternative<int>(result)) {
            std::cerr << "Error: Invalid move at move " << (i + 1) << std::endl;
            return 1;
        }
        
        auto captured = std::get<std::vector<Stone>>(result);
        if (!captured.empty()) {
            std::cout << "Captured " << captured.size() << " stone(s)" << std::endl;
        }
        
    }
    
    std::cout << "\nGame completed! Final position:" << std::endl;
    return 0;
}

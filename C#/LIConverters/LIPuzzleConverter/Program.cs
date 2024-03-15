using System;
using System.IO;
using System.Collections.Generic;

namespace LIPuzzleConverter
{
    class Program
    {
        static string convertLine(string[] s)
        {
            if (s.Length != 10)
                return "ERROR";
            string ret = "";
            for (int i = 0; i < 8; i++)
                ret += s[i] + ",";
            string op = s[9];
            if (op != "")
            {
                string[] s1 = op.Split(' ');
                if ((s1.Length==2)&&(s1[0]!=""))
                {
                    string s2 = s1[0] + "_";
                    if (s1[1].StartsWith(s2))
                        op = s1[0] + ": " + s1[1].Substring(s2.Length);
                }
                op = op.Replace('_', ' ');
            }
            ret += op;
            return ret;
        }

        static int convertint(string s)
        {
            int res = 0;
            int.TryParse(s, out res);
            return res;
        }

        static void Main(string[] args)
        {
            string inFile = "";
            string outFile = "";
            int minRating = 0;
            int maxRating = 9999;
            int minPopularity = 0;
            bool help = false;
            int maxItems = 10000;
            bool openings = false;

            for(int i = 0; i < args.Length; i++)
            {
                string arg = args[i];
                string uarg = arg.ToUpper();
                if (uarg.StartsWith("/"))
                    uarg = "-" + uarg.Substring(1);
                if (uarg.StartsWith("--"))
                    uarg = uarg.Substring(2);
                bool hasnext = i < args.Length - 1;
                if (uarg == "-IN")
                {
                    if (hasnext)
                        inFile = args[++i];
                    else
                        help = true;
                }
                else if (uarg == "-OUT")
                {
                    if (hasnext)
                        outFile = args[++i];
                    else
                        help = true;
                }
                else if (uarg == "-MINRATING")
                {
                    if (hasnext)
                        minRating = Convert.ToInt32(args[++i]);
                    else
                        help = true;
                }
                else if (uarg == "-MAXRATING")
                {
                    if (hasnext)
                        maxRating = Convert.ToInt32(args[++i]);
                    else
                        help = true;
                }
                else if (uarg == "-MINPOPULARITY")
                {
                    if (hasnext)
                        minPopularity = Convert.ToInt32(args[++i]);
                    else
                        help = true;
                }
                else if (uarg == "-MAXITEMS")
                {
                    if (hasnext)
                        maxItems = Convert.ToInt32(args[++i]);
                    else
                        help = true;
                }
                else if (uarg == "-OPENINGS")
                {
                    openings = true;
                }
                else if ((uarg == "-HELP") || (uarg == "-H") || (uarg == "?") || (uarg == "-?"))
                    help = true;
                else
                {
                    if (inFile == "")
                        inFile = arg;
                    else if (outFile == "")
                        outFile = arg;
                    else
                        help = true;
                }
                if (help)
                    break;
            }

            if (help)
            {
                Console.WriteLine("LIPuzzleConverter.exe [-in] INFILE [-maxitems XX] [-MinRating XX] [-MaxRating XX] [-MinPopularity XX] [-openings] [-out] OUTFILE");
                Console.WriteLine("  Max items is 10000 by default.");
                Environment.Exit(-1);
            }

            if ((inFile == "")||(!File.Exists(inFile)))
            {
                Console.WriteLine("Error - File not specified or does not exist!");
                Environment.Exit(-1);
            }

            if (outFile == "")
                outFile = inFile + ".out.csv";

            int lr = 0;
            int lw = 0;

            HashSet<int> moves = new HashSet<int>();
            HashSet<int> boards = new HashSet<int>();
            HashSet<int> opens = new HashSet<int>();

            try
            {
                StreamWriter of = new StreamWriter(outFile);
                using (FileStream fs = File.Open(inFile, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
                using (BufferedStream bs = new BufferedStream(fs))
                using (StreamReader sr = new StreamReader(bs))
                {
                    string line;
                    while ((line = sr.ReadLine()) != null)
                    {
                        lr++;
                        string[] s = line.Split(',');
                        if (s.Length!=10)
                        {
                            Console.Write("?");
                            continue;
                        }
                        bool includeit = true;
                        // pull out search terms
                        if (lr>1)
                        {
                            if (openings)
                                if (s[9] == "")
                                    includeit = false;
                            if (minRating > 0)
                                if (convertint(s[3]) < minRating)
                                    includeit = false;
                            if (maxRating < 9999)
                                if (convertint(s[3]) < maxRating)
                                    includeit = false;
                            if (minPopularity > 0)
                                if (convertint(s[5]) < minPopularity)
                                    includeit = false;
                            // look for repeats;
                            string board = s[1];
                            string move = s[2];
                            string opening = s[9];
                            int movh = move.GetHashCode();
                            if (moves.Contains(movh))
                                includeit = false;
                            else
                                moves.Add(movh);
                            if (opening == "")
                            {
                                int boardh = board.GetHashCode();
                                if (boards.Contains(boardh))
                                    includeit = false;
                                else
                                    boards.Add(boardh);
                            }
                            else
                            {
                                int openh = opening.GetHashCode();
                                if (opens.Contains(openh))
                                    includeit = false;
                                else
                                    opens.Add(openh);
                            }
                        }
                        if (!includeit)
                            continue;
                        line = convertLine(s);
                        if (lw++ >= maxItems)
                            break;
                        if (lw % 1000 == 0)
                            Console.Write(".");
                        of.WriteLine(line);
                    }
                }
                Console.WriteLine(lr.ToString() + " read, " + lw.ToString() + " written.");
                of.Close();
            }
            catch (Exception ex)
            {

                throw;
            }
        }
    }
}

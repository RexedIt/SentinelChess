using System;
using System.IO;
using System.Collections.Generic;

namespace LIPGNConverter
{
    class Program
    {

        static string FindSourceFolder(string subfolder)
        {
            string cd = Directory.GetCurrentDirectory();
            string lcd = Path.Combine(cd, subfolder);
            while(true)
            {
                if (Directory.Exists(lcd))
                    return lcd;
                var cdi = Directory.GetParent(cd);
                if (cdi == null)
                    return "";
                cd = cdi.ToString();
                if (cd == null)
                    return "";
                lcd = Path.Combine(cd, subfolder);
            }
        }

        static string FindSourceFile(string infile)
        {
            if (File.Exists(infile))
                return infile;
            string lcd = FindSourceFolder("SourceData");
            if (lcd != "")
                return Path.Combine(lcd,Path.GetFileName(infile));
            return infile;
        }

        static string FindDestFolder(string outDir)
        {
            if (Directory.Exists(outDir))
                return outDir;
            string lcd = FindSourceFolder("DestData");
            if (lcd != "")
            {
                string candidate = Path.Combine(lcd,Path.GetFileName(outDir));
                if (!Directory.Exists(candidate))
                    Directory.CreateDirectory(candidate);
                return candidate;
            }
            return outDir;
        }

        static int ctr = 0;
        static bool Save(string outDir, List<string> lines, bool guid, int startat)
        {
            try
            {
                int idx = (ctr++) + startat;
                string fileName = (idx).ToString("D8") + ".pgn";
                if (guid)
                    fileName = Guid.NewGuid().ToString() + ".pgn";
                fileName = Path.Combine(outDir, fileName);
                while (lines[0] == "")
                    lines.RemoveAt(0);
                while (lines[lines.Count-1].Trim() == "")
                    lines.RemoveAt(lines.Count-1);
                string contents = String.Join(Environment.NewLine, lines);
                File.WriteAllText(fileName,contents);
                return true;
            }
            catch (System.Exception ex)
            {
                Console.WriteLine("*** Error - " + ex.Message);
                return false;
            }

        }

        static string strip(string orig, char beg, char end)
        {
            if (orig.StartsWith(beg))
            {
                if (orig.EndsWith(end))
                {
                    return orig.Substring(1).Substring(0, orig.Length-2);
                }
            }
            return orig;
        }

        static bool Filter(List<string> lines, int minRating, int maxRating, string user, string eventname)
        {
            if (lines.Count==0)
                return false;
            if ((minRating == 0) && (maxRating == 9999) && (user == "") && (eventname == ""))
                return true;
            string _eventname = "";
            int whiteRating = 0;
            int blackRating = 0;
            string blackUser = "";
            string whiteUser = "";

            foreach(string line in lines)
            {
                if (line.StartsWith('['))
                {
                    if (line.EndsWith(']'))
                    {
                        string stripped = strip(line, '[', ']');
                        int l = stripped.IndexOf('"');
                        if (l==-1)
                            continue;
                        string lhs = stripped.Substring(0,l).Trim();
                        string rhs = strip(stripped.Substring(l),'"','"');
                        if (lhs.ToUpper() == "WHITE")
                            whiteUser = rhs;
                        else if (lhs.ToUpper() == "BLACK")
                            blackUser = rhs;
                        else if (lhs.ToUpper() == "WHITEELO")
                            int.TryParse(rhs, out whiteRating); 
                        else if (lhs.ToUpper() == "BLACKELO")
                            int.TryParse(rhs, out blackRating); 
                        else if (lhs.ToUpper() == "EVENT")
                            _eventname = rhs;
                    }
                }
            }

            if (eventname != "")
            {
                if (!_eventname.ToUpper().Contains(eventname.ToUpper()))
                    return false;
            }

            if (user != "")
            {
                if (blackUser.ToUpper() != user.ToUpper())
                  if (whiteUser.ToUpper() != user.ToUpper())
                    return false;
            }

            if (minRating > 0)
            {
                if (whiteRating < minRating)
                    if (blackRating < minRating)
                        return false;
            }

            if (maxRating > 0)
            {
                if (whiteRating > maxRating)
                    if (blackRating > maxRating)
                        return false;
            }

            return true;
        }

        static void Main(string[] args)
        {
            string inFile = "";
            string outDir = "";
            int minRating = 0;
            int maxRating = 9999;
            string user = "";
            string eventname = "";
            bool help = false;
            int maxItems = 100;
            bool count = false;
            bool guid = false;
            int startat = 1;

            for(int i = 0; i < args.Length; i++)
            {
                string arg = args[i];
                string uarg = arg.ToUpper();
                if (uarg.StartsWith("/"))
                    uarg = "-" + uarg.Substring(1);
                if (uarg.StartsWith("--"))
                    uarg = uarg.Substring(1);
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
                        outDir = args[++i];
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
                else if (uarg == "-USER")
                {
                    if (hasnext)
                        user = args[++i];
                    else
                        help = true;
                }
                else if (uarg == "-EVENT")
                {
                    if (hasnext)
                        eventname = args[++i];
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
                else if (uarg == "-GUID")
                {
                    guid = true;
                }
                else if (uarg == "-COUNT")
                {
                    count = true;
                }
                else if (uarg == "-STARTAT")
                {
                    if (hasnext)
                        startat = Convert.ToInt32(args[++i]);
                    else
                        help = true;
                }
                else if ((uarg == "-HELP") || (uarg == "-H") || (uarg == "?") || (uarg == "-?"))
                    help = true;
                else
                {
                    if (inFile == "")
                        inFile = arg;
                    else if (outDir == "")
                        outDir = arg;
                    else
                        help = true;
                }
                if (help)
                    break;
            }

            if (help)
            {
                Console.WriteLine("LIPGNConverter.exe [-in] INFILE [-maxitems XX] [-MinRating XX] [-MaxRating XX] [-User XX] [-Event XX] [-guid] [-out] OUTDIR");
                Console.WriteLine("  Max items is 100 by default.");
                Environment.Exit(-1);
            }

            inFile = FindSourceFile(inFile);
            if ((inFile == "")||(!File.Exists(inFile)))
            {
                Console.WriteLine("Error - File not specified or does not exist!");
                Environment.Exit(-1);
            }

            outDir = FindDestFolder(outDir);
            if (outDir == "")
            {
                Console.WriteLine("Error - Output Directory is not specified!");
                Environment.Exit(-1);
            }

            if (!Directory.Exists(outDir))
            {
                Console.WriteLine("Error - Output Directory is not found!");
                Environment.Exit(-1);
            }

            int lr = 0;
            int lw = 0;

            try
            {
                using (FileStream fs = File.Open(inFile, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
                using (BufferedStream bs = new BufferedStream(fs))
                using (StreamReader sr = new StreamReader(bs))
                {
                    List<string> lines = new List<string>();
                    string line;
                    while ((line = sr.ReadLine()) != null)
                    {
                        lr++;
                        if (line.StartsWith("[Event "))
                        {
                            if (Filter(lines, minRating, maxRating, user, eventname))
                            {
                                if ((!count)&&(!Save(outDir, lines, guid, startat)))
                                    Environment.Exit(-1);
                                else
                                    lw ++;
                                if (lw >= maxItems)
                                    break;
                                if (lw % 1000 == 0)
                                    Console.Write(".");
                            }
                            lines.Clear();
                        }
                        lines.Add(line.Trim());
                    }
                }
                Console.WriteLine("\r\n" + lr.ToString() + " read, " + lw.ToString() + " written.\r\n");
            }
            catch (Exception ex)
            {
            }
        }
    }
}

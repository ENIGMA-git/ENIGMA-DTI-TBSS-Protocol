#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#                        %%%  ENIGMA DTI %%%
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#%% This is a function to print out images for Quality Control
#%% of DTI_ENIGMA FA images with TBSS (FSL) skeltons overlaid
#%% as well as JHU atlas ROIs
#%%
#%% Please QC your images to make sure they are
#%% correct FA maps and oriented and aligned properly
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#%% Written by Aarya Vakharia, Talia M. Nir, Elizabeth Haddad, Neda Jahanshad
#%%   last update April 2026 (Based on 2014 scripts by Derrek Hibar)
#%%           Questions or Comments??
#%% neda.jahanshad@ini.usc.edu / derrek.hibar@ini.usc.edu
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


# -----------------------------------------------------------------------------
# 1. Configuration & Argument Parsing
# -----------------------------------------------------------------------------

# --- Default Settings ---
cmdargs <- commandArgs(trailingOnly = TRUE)

site          <- "My Cohort"
CSVfile       <- NULL
Nrois         <- 63
rois_string   <- "AverageFA;BCC;GCC;SCC;CC;CGC;CGH;CR;EC;FX;FXST;IC;IFO;PTR;SFO;SLF;SS;UNC;CST;ACR;ALIC;PCR;PLIC;RLIC;SCR;ACR.L;ACR.R;ALIC.L;ALIC.R;CGC.L;CGC.R;CGH.L;CGH.R;CR.L;CR.R;CST.L;CST.R;EC.L;EC.R;FX.ST.L;FX.ST.R;IC.L;IC.R;IFO.L;IFO.R;PCR.L;PCR.R;PLIC.L;PLIC.R;PTR.L;PTR.R;RLIC.L;RLIC.R;SCR.L;SCR.R;SFO.L;SFO.R;SLF.L;SLF.R;SS.L;SS.R;UNC.L;UNC.R"
hbins         <- 20

output_format <- "both"
outD          <- "QC_ENIGMA"
template_file <- "template.Rmd"


# --- Helper: Parse Command-Line Arguments ---
parse_args <- function(cmdargs) {
  
  if (!("--file" %in% cmdargs)) {
    stop("Error: Please provide a file path using the --file flag. Example: --file /path/to/combinedROItable_FA.csv")
  }
  
  local_outdir <- "." 
  
  for (i in seq_along(cmdargs)) {
    arg <- cmdargs[i]
    
    # Site name
    if(length(cmdargs) == 1 && !grepl("^--", cmdargs[1])){
      site <<- cmdargs[1]
    }
    if (arg == "--cohort" && i < length(cmdargs)) {
      site <<- cmdargs[i + 1]
    }
    
    # CSV file path
    if (arg == "--file" && i < length(cmdargs)) {
      CSVfile <<- cmdargs[i + 1]
    }
    
    # Output Directory Path
    if (arg == "--outdir" && i < length(cmdargs)) {
      local_outdir <- cmdargs[i + 1]
    }
    
    # HTML template (only valid for HTML)
    if (arg == "--template" && i < length(cmdargs)) {
      if (output_format == "pdf") {
        stop(paste0("Cannot use --template flag with format: ", output_format))
      } else {
        template_file <<- cmdargs[i + 1]
      }
    }
  }
  
  if (basename(local_outdir) == "QC_ENIGMA") {
    final_outD <- local_outdir
  } else {
    final_outD <- file.path(local_outdir, "QC_ENIGMA")
  }
  
  outD <<- final_outD
  
  if (!dir.exists(outD)) {
    dir.create(outD, recursive = TRUE)
    cat("Created output directory:", outD, "\n")
  }
}



# Apply command-line overrides
if (length(cmdargs) > 0) parse_args(cmdargs)

filename_only <- basename(CSVfile)

# Remove extension and add suffix
outTXT        <- paste0(tools::file_path_sans_ext(filename_only), "_stats.txt")
base_filename <- paste0(tools::file_path_sans_ext(filename_only), "_histogram")

# --- Output Paths ---
outPDF <- file.path(outD, paste0(base_filename, ".pdf"))
outHTML <- file.path(outD, paste0(base_filename, ".html"))


# -----------------------------------------------------------------------------
# 2. Data Loading & Cleaning
# -----------------------------------------------------------------------------

# --- Helper: Clean Table Data ---
clean_table <- function(Table) {
  colTable <- names(Table)
  
  # Replace 'x'/'X' with NA
  for (col_name in colTable) {
    ind_x <- which(Table[[col_name]] %in% c("x", "X"))
    if (length(ind_x) > 0) Table[ind_x, col_name] <- NA
  }
  
  # Convert ROI columns to numeric
  rois <- strsplit(rois_string, ";")[[1]]
  for (roi_name in rois) {
    if (roi_name %in% names(Table)) {
      Table[[roi_name]] <- suppressWarnings(as.numeric(Table[[roi_name]]))
    } else {
      warning(paste0("ROI '", roi_name, "' not found in CSV columns. It will be skipped."))
    }
  }
  
  na.omit(Table)
}

# --- Load Data ---
Table <- read.csv(CSVfile, header = TRUE, stringsAsFactors = FALSE)
Table <- clean_table(Table)

rois_list_vector <- strsplit(rois_string, ";")[[1]]


# -----------------------------------------------------------------------------
# 3. Statistics Table Generation
# -----------------------------------------------------------------------------

generate_stats_table <- function(Table, rois_list, out_path) {
  write("Structure\tNumberIncluded\tMean\tStandDev\tMedian\tInterquartileRange\tMaxValue\tMinValue\tMaxSubject\tMinSubject\t5StDev_Off", 
        file = out_path)
  
  for (ROI in rois_list) {
    if (!ROI %in% names(Table)) {
      warning(paste0("ROI '", ROI, "' not found in data for stats. Skipping..."))
      next
    }
    
    DATA <- Table[[ROI]]
    if (!is.numeric(DATA) || all(is.na(DATA))) {
      warning(paste0("ROI '", ROI, "' contains only NA/non-numeric values. Skipping."))
      next
    }
    
    mu <- mean(DATA, na.rm = TRUE)
    sdev <- sd(DATA, na.rm = TRUE)
    median <- median(DATA, na.rm = TRUE)
    iqr <- IQR(DATA, na.rm = TRUE)
    N <- length(DATA)
    maxV <- max(DATA)
    minV <- min(DATA)
    
    maxSubj <- paste(unique(Table[Table[[ROI]] == maxV, 1]), collapse = ", ")
    minSubj <- paste(unique(Table[Table[[ROI]] == minV, 1]), collapse = ", ")
    
    # Outliers beyond 5 SD
    minO_idx <- which(DATA < mu - 5 * sdev)
    maxO_idx <- which(DATA > mu + 5 * sdev)
    outliers <- ""
    if (length(minO_idx) + length(maxO_idx) > 0) {
      subjects <- unique(c(as.character(Table[minO_idx, 1]), as.character(Table[maxO_idx, 1])))
      outliers <- paste("Outliers (5-sd):", paste(subjects, collapse = ","))
    }
    
    stats_vec <- c(ROI, N, mu, sdev, median, iqr, maxV, minV, maxSubj, minSubj, outliers)
    write.table(t(as.matrix(stats_vec)), file = out_path, append = TRUE, 
                quote = FALSE, col.names = FALSE, row.names = FALSE, sep = "\t")
  }
  
  message(paste0("Statistics saved to: ", out_path))
}

# Run stats if applicable
if (Nrois > 0 && nrow(Table) > 0) {
  generate_stats_table(Table, rois_list_vector, file.path(outD, outTXT))
}


# -----------------------------------------------------------------------------
# 4. Report Generation
# -----------------------------------------------------------------------------

# --- PDF Report ---
generate_pdf_report <- function(Table, rois_list, out_path, site_name) {
  message("Generating PDF...")
  pdf(file = out_path)
  for (ROI in rois_list) {
    if (ROI %in% names(Table)) {
      DATA = Table[[ROI]] 
      hist(DATA, breaks = hbins, main = paste(site_name, ROI))
    }
  }
  dev.off()
  message(paste0("PDF report successfully generated at: ", out_path))
}

# --- HTML Report ---
generate_html_report <- function(Table, rois_list, out_path, template_path) {
  message("Generating interactive HTML...")
  
  if (!requireNamespace("rmarkdown", quietly = TRUE))
    install.packages("rmarkdown", repos = "http://cran.us.r-project.org")
  if (!requireNamespace("plotly", quietly = TRUE))
    install.packages("plotly", repos = "http://cran.us.r-project.org")
  
  library(rmarkdown)
  library(plotly)
  
  if (!file.exists(template_path)) {
    stop(paste0("Error: template.Rmd not found at: ", template_path))
  }
  
  render(
    input = template_path,
    output_file = basename(out_path),
    output_dir = dirname(out_path),
    params = list(
      site = site,
      rois_list = list(value = rois_list),
      data_table = Table,
      hbins = hbins
    ),
    envir = new.env()
  )
  
  message(paste0("Interactive HTML report successfully generated at: ", out_path))
}


# -----------------------------------------------------------------------------
# 5. Run Selected Report Type
# -----------------------------------------------------------------------------

if (output_format == "pdf" || output_format == "both") {
  generate_pdf_report(Table, rois_list_vector, outPDF, site)
}

if (output_format == "html" || output_format == "both") {
  generate_html_report(Table, rois_list_vector, outHTML, template_file)
}
